
/* Copyright (c) 2017, Human Brain Project
 *                          Stefan.Eilemann@epfl.ch
 */

#include "monitor.h"

#include "detail/constants.h"
#include "detail/context.h"
#include "detail/socket.h"
#include "log.h"
#include "publisher.h"

#include <zmq.h>

namespace zeroeq
{
class Monitor::Impl
{
public:
    Impl() {}
    virtual ~Impl() {}
    void addSockets(std::vector<zeroeq::detail::Socket>& entries)
    {
        zeroeq::detail::Socket entry;
        entry.socket = _socket.get();
        entry.events = ZMQ_POLLIN;
        entries.push_back(entry);
    }

    virtual bool process(void* socket, Monitor& monitor) = 0;

protected:
    zmq::SocketPtr _socket;
};

namespace
{
class XPubImpl : public Monitor::Impl
{
public:
    XPubImpl(Sender& sender)
    {
        _socket = sender.getSocket();

        const int on = 1;
        if (zmq_setsockopt(_socket.get(), ZMQ_XPUB_VERBOSE, &on, sizeof(on)) ==
            -1)
        {
            ZEROEQTHROW(std::runtime_error(
                std::string("Enabling ZMQ_XPUB_VERBOSE failed: ") +
                zmq_strerror(zmq_errno())));
        }
    }

    ~XPubImpl()
    {
        const int off = 0;
        zmq_setsockopt(_socket.get(), ZMQ_XPUB_VERBOSE, &off, sizeof(off));
    }

    bool process(void* socket, Monitor& monitor)
    {
        // Message event is one byte 0=unsub or 1=sub, followed by topic
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        if (zmq_msg_recv(&msg, socket, 0) == -1)
            return false;

        const uint8_t* data = (const uint8_t*)zmq_msg_data(&msg);
        switch (*data)
        {
        case 0:
            break; // unsub

        case 1: // sub
            if (zmq_msg_size(&msg) == sizeof(uint8_t) + sizeof(uint128_t) &&
                *(const uint128_t*)(data + 1) == MEERKAT) // new subscriber
            {
                monitor.notifyNewConnection();
                return true;
            }
            break;

        default:
            ZEROEQWARN << "Unhandled monitor event" << std::endl;
        }
        zmq_msg_close(&msg);
        return false;
    }
};

class SocketImpl : public Monitor::Impl
{
public:
    SocketImpl(Sender& sender)
        : _context(detail::getContext())
    {
#if (ZMQ_VERSION < 40104)
        ZEROEQTHROW(std::runtime_error(
            "ZeroEQ version with bug in socket monitor, need at least 4.1.4"));
#endif
        const auto inproc = std::string("inproc://zeroeq.monitor.") +
                            servus::make_UUID().getString();

        if (::zmq_socket_monitor(sender.getSocket().get(), inproc.c_str(),
                                 ZMQ_EVENT_ALL) != 0)
        {
            ZEROEQTHROW(
                std::runtime_error(std::string("Cannot monitor socket: ") +
                                   zmq_strerror(zmq_errno())));
        }

        _socket.reset(::zmq_socket(_context.get(), ZMQ_PAIR),
                      [](void* s) { ::zmq_close(s); });
        if (!_socket)
            ZEROEQTHROW(std::runtime_error(
                std::string("Cannot create inproc socket: ") +
                zmq_strerror(zmq_errno())));

        if (::zmq_connect(_socket.get(), inproc.c_str()) != 0)
        {
            ZEROEQTHROW(std::runtime_error(
                std::string("Cannot connect inproc socket: ") +
                zmq_strerror(zmq_errno())));
        }
    }

    ~SocketImpl() {}
    bool process(void* socket, Monitor& monitor)
    {
        // Messages consist of 2 Frames, the first containing the event-id and
        // the associated value. The second frame holds the affected endpoint as
        // string.
        zmq_msg_t msg;
        zmq_msg_init(&msg);

        //  The layout of the first Frame is: 16 bit event id 32 bit event value
        if (zmq_msg_recv(&msg, socket, 0) == -1)
        {
            ZEROEQWARN << "Can't read event id from monitor socket"
                       << std::endl;
            return false;
        }
        const uint16_t event = *(uint16_t*)zmq_msg_data(&msg);
        // Ignore event value

        if (zmq_msg_more(&msg))
        {
            zmq_msg_close(&msg);

            //  Second frame in message contains event address, skip
            zmq_msg_init(&msg);
            if (zmq_msg_recv(&msg, socket, 0) == -1)
                ZEROEQWARN << "Can't read address from monitor socket"
                           << std::endl;
        }
        else
            ZEROEQWARN << "Monitor event has no event address" << std::endl;

        zmq_msg_close(&msg);

        switch (event)
        {
        case ZMQ_EVENT_CONNECTED:
        case ZMQ_EVENT_ACCEPTED:
            monitor.notifyNewConnection();
            return true;

        default:
            ZEROEQWARN << "Unhandled monitor event " << event << std::endl;
        }
        return false;
    }

private:
    zmq::ContextPtr _context;
};

Monitor::Impl* newImpl(Sender& sender)
{
    if (dynamic_cast<Publisher*>(&sender))
        return new XPubImpl(sender);
    return new SocketImpl(sender);
}
}

Monitor::Monitor(Sender& sender)
    : Receiver()
    , _impl(newImpl(sender))
{
}

Monitor::Monitor(Sender& sender, Receiver& shared)
    : Receiver(shared)
    , _impl(newImpl(sender))
{
}

Monitor::~Monitor()
{
}

void Monitor::addSockets(std::vector<zeroeq::detail::Socket>& entries)
{
    _impl->addSockets(entries);
}

bool Monitor::process(zeroeq::detail::Socket& socket)
{
    return _impl->process(socket.socket, *this);
}
}
