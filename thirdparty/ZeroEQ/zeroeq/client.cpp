
/* Copyright (c) 2017, Human Brain Project
 *                     Stefan.Eilemann@epfl.ch
 */

#include "client.h"

#include "detail/common.h"
#include "detail/receiver.h"

#include <servus/servus.h>
#include <thread>
#include <unordered_map>

namespace zeroeq
{
class Client::Impl : public detail::Receiver
{
public:
    explicit Impl(const std::string& session)
        : detail::Receiver(SERVER_SERVICE, session == DEFAULT_SESSION
                                               ? getDefaultRepSession()
                                               : session)
        , _servers(zmq_socket(getContext(), ZMQ_DEALER),
                   [](void* s) { ::zmq_close(s); })
    {
        const char* serversEnv = getenv("ZEROEQ_SERVERS");
        if (!serversEnv)
            return;

        std::string servers(serversEnv);
        while (!servers.empty())
        {
            const size_t pos = servers.find(',');
            const std::string server = servers.substr(0, pos);
            servers = pos == std::string::npos ? std::string()
                                               : servers.substr(pos + 1);

            const auto& zmqURI = buildZmqURI(URI(server));
            if (!addConnection(zmqURI))
                ZEROEQTHROW(std::runtime_error("Cannot connect client to " +
                                               zmqURI + ": " +
                                               zmq_strerror(zmq_errno())));
        }

        update();
    }

    explicit Impl(const URIs& uris)
        : detail::Receiver(SERVER_SERVICE)
        , _servers(zmq_socket(getContext(), ZMQ_DEALER),
                   [](void* s) { ::zmq_close(s); })
    {
        for (const auto& uri : uris)
        {
            if (!uri.isFullyQualified())
                ZEROEQTHROW(std::runtime_error(
                    std::string("Non-fully qualified URI used for server")));

            const auto& zmqURI = buildZmqURI(uri);
            if (!addConnection(zmqURI))
                ZEROEQTHROW(std::runtime_error("Cannot connect client to " +
                                               zmqURI + ": " +
                                               zmq_strerror(zmq_errno())));
        }
    }

    ~Impl() {}

    zmq::SocketPtr createSocket(const uint128_t&) final { return _servers; }

    bool request(uint128_t requestID, const void* data, const size_t size,
                 const ReplyFunc& func)
    {
        const bool hasPayload = data && size > 0;
        ++_id;
#ifdef ZEROEQ_BIGENDIAN
        detail::byteswap(requestID); // convert to little endian wire protocol
#endif

        if (!_send(&_id, sizeof(_id), ZMQ_SNDMORE) ||
            !_send(nullptr, 0, ZMQ_SNDMORE) || // frame delimiter
            !_send(&requestID, sizeof(requestID), hasPayload ? ZMQ_SNDMORE : 0))
        {
            return false;
        }

        if (hasPayload && !_send(data, size, 0))
            return false;

        _handlers[_id] = func;
        return true;
    }

    bool process(detail::Socket& socket)
    {
        uint64_t id;
        uint128_t replyID;

        if (!_recv(&id, sizeof(id), ZMQ_DONTWAIT) || !_recv(nullptr, 0, 0))
            return false;
        const bool payload = _recv(&replyID, sizeof(replyID), 0);

#ifdef ZEROEQ_BIGENDIAN
        detail::byteswap(replyID); // convert to little endian wire protocol
#endif

        zmq_msg_t msg;
        if (payload)
        {
            zmq_msg_init(&msg);
            zmq_msg_recv(&msg, socket.socket, 0);
        }

        auto i = _handlers.find(id);
        if (i == _handlers.cend())
        {
            if (payload)
                zmq_msg_close(&msg);

            ZEROEQTHROW(std::runtime_error("Got unrequested reply " +
                                           std::to_string(id)));
        }

        if (payload)
        {
            i->second(replyID, zmq_msg_data(&msg), zmq_msg_size(&msg));
            zmq_msg_close(&msg);
        }
        else
            i->second(replyID, nullptr, 0);
        _handlers.erase(i);
        return true;
    }

private:
    bool _send(const void* data, const size_t size, int flags)
    {
        zmq_msg_t msg;
        zmq_msg_init_size(&msg, size);
        if (data)
            ::memcpy(zmq_msg_data(&msg), data, size);

        flags |= ZMQ_DONTWAIT;
        while (true)
        {
            const int ret = zmq_msg_send(&msg, _servers.get(), flags);
            if (ret == -1 && zmq_errno() == EAGAIN)
            {
                if (!update())
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            else
            {
                zmq_msg_close(&msg);

                if (ret != -1)
                    return true;

                ZEROEQWARN << "Cannot send request: "
                           << zmq_strerror(zmq_errno()) << std::endl;
                return false;
            }
        }
    }

    /** @return true if more data available */
    bool _recv(void* data, const size_t size, const int flags)
    {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        if (zmq_msg_recv(&msg, _servers.get(), flags) == -1)
            return false;

        if (zmq_msg_size(&msg) != size)
            ZEROEQWARN << "Reply size mismatch, expected " << size << " got "
                       << zmq_msg_size(&msg) << std::endl;
        else if (data)
            ::memcpy(data, zmq_msg_data(&msg), size);
        const bool more = zmq_msg_more(&msg);
        zmq_msg_close(&msg);
        return more;
    }

    zmq::SocketPtr _servers;
    std::unordered_map<uint64_t, ReplyFunc> _handlers;
    uint64_t _id{0};
};

Client::Client()
    : Receiver()
    , _impl(new Impl(DEFAULT_SESSION))
{
}

Client::Client(const std::string& session)
    : Receiver()
    , _impl(new Impl(session))
{
}

Client::Client(const URIs& uris)
    : Receiver()
    , _impl(new Impl(uris))
{
}

Client::Client(Receiver& shared)
    : Receiver(shared)
    , _impl(new Impl(DEFAULT_SESSION))
{
}

Client::Client(const std::string& session, Receiver& shared)
    : Receiver(shared)
    , _impl(new Impl(session))
{
}

Client::Client(const URIs& uris, Receiver& shared)
    : Receiver(shared)
    , _impl(new Impl(uris))
{
}

Client::~Client()
{
}

bool Client::request(const servus::Serializable& req, const ReplyFunc& func)
{
    const auto& data = req.toBinary();
    return request(req.getTypeIdentifier(), data.ptr.get(), data.size, func);
}

bool Client::request(const uint128_t& requestID, const void* data,
                     const size_t size, const ReplyFunc& func)
{
    return _impl->request(requestID, data, size, func);
}

const std::string& Client::getSession() const
{
    return _impl->getSession();
}

void Client::addSockets(std::vector<detail::Socket>& entries)
{
    _impl->addSockets(entries);
}

bool Client::process(detail::Socket& socket)
{
    return _impl->process(socket);
}

void Client::update()
{
    _impl->update();
}

void Client::addConnection(const std::string& uri)
{
    _impl->addConnection(uri);
}
}
