
/* Copyright (c) 2014-2017, Human Brain Project
 *                          Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *                          Stefan.Eilemann@epfl.ch
 */

#include "subscriber.h"

#include "detail/byteswap.h"
#include "detail/common.h"
#include "detail/constants.h"
#include "detail/receiver.h"
#include "detail/sender.h"
#include "detail/socket.h"
#include "log.h"

#include <servus/serializable.h>
#include <servus/servus.h>

#include <cassert>
#include <cstring>
#include <map>
#include <stdexcept>

namespace zeroeq
{
class Subscriber::Impl : public detail::Receiver
{
public:
    Impl(const std::string& session)
        : detail::Receiver(PUBLISHER_SERVICE, session == DEFAULT_SESSION
                                                  ? getDefaultPubSession()
                                                  : session)
        , _selfInstance(detail::Sender::getUUID())
    {
        update();
    }

    Impl(const URIs& uris)
        : detail::Receiver(PUBLISHER_SERVICE)
        , _selfInstance(detail::Sender::getUUID())
    {
        for (const URI& uri : uris)
        {
            if (!uri.isFullyQualified())
                ZEROEQTHROW(std::runtime_error(std::string(
                    "Non-fully qualified URI used for subscriber")));

            const std::string& zmqURI = buildZmqURI(uri);
            if (!addConnection(zmqURI))
            {
                ZEROEQTHROW(std::runtime_error("Cannot connect subscriber to " +
                                               zmqURI + ": " +
                                               zmq_strerror(zmq_errno())));
            }
        }
    }

    bool subscribe(servus::Serializable& serializable)
    {
        const auto func = [&serializable](const void* data, const size_t size) {
            serializable.fromBinary(data, size);
        };
        return subscribe(serializable.getTypeIdentifier(), func);
    }

    bool subscribe(const uint128_t& event, const EventPayloadFunc& func)
    {
        if (_eventFuncs.count(event) != 0)
            return false;

        _subscribe(event);
        _eventFuncs[event] = func;
        return true;
    }

    bool unsubscribe(const servus::Serializable& serializable)
    {
        return unsubscribe(serializable.getTypeIdentifier());
    }

    bool unsubscribe(const uint128_t& event)
    {
        if (_eventFuncs.erase(event) == 0)
            return false;

        _unsubscribe(event);
        return true;
    }

    bool process(detail::Socket& socket)
    {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        zmq_msg_recv(&msg, socket.socket, 0);

        uint128_t type;
        memcpy(&type, zmq_msg_data(&msg), sizeof(type));
#ifndef ZEROEQ_LITTLEENDIAN
        detail::byteswap(type); // convert from little endian wire
#endif
        const bool payload = zmq_msg_more(&msg);
        zmq_msg_close(&msg);

        if (payload)
        {
            zmq_msg_init(&msg);
            zmq_msg_recv(&msg, socket.socket, 0);
        }

        EventFuncMap::const_iterator i = _eventFuncs.find(type);
        if (i == _eventFuncs.cend())
        {
            if (payload)
                zmq_msg_close(&msg);

            ZEROEQTHROW(std::runtime_error("Got unsubscribed event " +
                                           type.getString()));
        }

        if (payload)
        {
            i->second(zmq_msg_data(&msg), zmq_msg_size(&msg));
            zmq_msg_close(&msg);
        }
        else
            i->second(nullptr, 0);
        return true;
    }

    zmq::SocketPtr createSocket(const uint128_t& instance)
    {
        if (instance == _selfInstance)
            return {};

        zmq::SocketPtr socket(zmq_socket(getContext(), ZMQ_SUB),
                              [](void* s) { ::zmq_close(s); });
        const int hwm = 0;
        zmq_setsockopt(socket.get(), ZMQ_RCVHWM, &hwm, sizeof(hwm));

        // Tell a Monitor on a Publisher we're here
        if (zmq_setsockopt(socket.get(), ZMQ_SUBSCRIBE, &MEERKAT,
                           sizeof(uint128_t)) == -1)
        {
            ZEROEQTHROW(std::runtime_error(
                std::string("Cannot update meerkat filter: ") +
                zmq_strerror(zmq_errno())));
        }

        // Add existing subscriptions to socket
        for (const auto& i : _eventFuncs)
        {
            if (zmq_setsockopt(socket.get(), ZMQ_SUBSCRIBE, &i.first,
                               sizeof(uint128_t)) == -1)
            {
                ZEROEQTHROW(std::runtime_error(
                    std::string("Cannot update topic filter: ") +
                    zmq_strerror(zmq_errno())));
            }
        }
        return socket;
    }

private:
    typedef std::map<uint128_t, EventPayloadFunc> EventFuncMap;
    EventFuncMap _eventFuncs;

    const uint128_t _selfInstance;

    void _subscribe(const uint128_t& event)
    {
        for (const auto& socket : getSockets())
        {
            if (zmq_setsockopt(socket.second.get(), ZMQ_SUBSCRIBE, &event,
                               sizeof(event)) == -1)
            {
                ZEROEQTHROW(std::runtime_error(
                    std::string("Cannot update topic filter: ") +
                    zmq_strerror(zmq_errno())));
            }
        }
    }

    void _unsubscribe(const uint128_t& event)
    {
        for (const auto& socket : getSockets())
        {
            if (zmq_setsockopt(socket.second.get(), ZMQ_UNSUBSCRIBE, &event,
                               sizeof(event)) == -1)
            {
                ZEROEQTHROW(std::runtime_error(
                    std::string("Cannot update topic filter: ") +
                    zmq_strerror(zmq_errno())));
            }
        }
    }
};

Subscriber::Subscriber()
    : Receiver()
    , _impl(new Impl(DEFAULT_SESSION))
{
}

Subscriber::Subscriber(const std::string& session)
    : Receiver()
    , _impl(new Impl(session))
{
}

Subscriber::Subscriber(const URIs& uris)
    : Receiver()
    , _impl(new Impl(uris))
{
}

Subscriber::Subscriber(Receiver& shared)
    : Receiver(shared)
    , _impl(new Impl(DEFAULT_SESSION))
{
}

Subscriber::Subscriber(const std::string& session, Receiver& shared)
    : Receiver(shared)
    , _impl(new Impl(session))
{
}

Subscriber::Subscriber(const URIs& uris, Receiver& shared)
    : Receiver(shared)
    , _impl(new Impl(uris))
{
}

Subscriber::~Subscriber()
{
}

bool Subscriber::subscribe(servus::Serializable& serializable)
{
    return _impl->subscribe(serializable);
}

bool Subscriber::subscribe(const uint128_t& event, const EventFunc& func)
{
    return _impl->subscribe(event, [func](const void*, size_t) { func(); });
}

bool Subscriber::subscribe(const uint128_t& event, const EventPayloadFunc& func)
{
    return _impl->subscribe(event, func);
}

bool Subscriber::unsubscribe(const servus::Serializable& serializable)
{
    return _impl->unsubscribe(serializable);
}

bool Subscriber::unsubscribe(const uint128_t& event)
{
    return _impl->unsubscribe(event);
}

const std::string& Subscriber::getSession() const
{
    return _impl->getSession();
}

void Subscriber::addSockets(std::vector<detail::Socket>& entries)
{
    _impl->addSockets(entries);
}

bool Subscriber::process(detail::Socket& socket)
{
    return _impl->process(socket);
}

void Subscriber::update()
{
    _impl->update();
}

void Subscriber::addConnection(const std::string& uri)
{
    _impl->addConnection(uri);
}
}
