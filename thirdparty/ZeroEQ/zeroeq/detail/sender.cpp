
/* Copyright (c) 2015-2017, Human Brain Project
 *                          Stefan.Eilemann@epfl.ch
 */

#include "sender.h"

#include "../log.h"
#include "common.h"
#include "constants.h"
#include "context.h"
#include "socket.h"

#include <zmq.h>

// for NI_MAXHOST
#ifdef _WIN32
#include <Ws2tcpip.h>
#else
#include <netdb.h>
#include <unistd.h>
#endif

namespace zeroeq
{
namespace detail
{
Sender::Sender(const URI& uri_, const int type)
    : Sender(uri_, type, {}, {})
{
}

Sender::Sender(const URI& uri_, const int type, const std::string service,
               const std::string& session)
    : _context(getContext())
    , uri(uri_)
    , socket(zmq_socket(_context.get(), type), [](void* s) { ::zmq_close(s); })
    , _service(session == TEST_SESSION ? session : service)
    , _session(session)
{
    const int hwm = 0;
    zmq_setsockopt(socket.get(), ZMQ_SNDHWM, &hwm, sizeof(hwm));
}

Sender::~Sender()
{
    socket.reset();
}

std::string Sender::getAddress() const
{
    return uri.getHost() + ":" + std::to_string(uint32_t(uri.getPort()));
}

void Sender::initURI()
{
    if (uri.getScheme() != DEFAULT_SCHEMA)
        return;

    std::string host = uri.getHost();
    if (host == "*")
        host.clear();

    uint16_t port = uri.getPort();
    if (!host.empty() && port != 0)
        return;

    std::string hostStr, portStr;
    _getEndPoint(hostStr, portStr);

    if (port == 0)
    {
        // No overflow is possible unless ZMQ reports bad port number.
        port = std::stoi(portStr);
        uri.setPort(port);
    }

    if (host.empty())
        uri.setHost(hostStr);

    ZEROEQINFO << "Bound to " << uri << std::endl;
}

void Sender::announce()
{
    if (!servus::Servus::isAvailable())
    {
        ZEROEQWARN << "ZeroEQ::Sender: Cannot announce on Zeroconf; no "
                      "implementation provided by Servus"
                   << std::endl;
        return;
    }

    _service.set("Type", "ZeroEQ");
    _service.set(KEY_INSTANCE, getUUID().getString());
    _service.set(KEY_USER, getUserName());
    _service.set(KEY_APPLICATION, getApplicationName());
    if (!_session.empty())
        _service.set(KEY_SESSION, _session);

    const auto& result = _service.announce(uri.getPort(), getAddress());
    if (result == servus::Servus::Result::NOT_SUPPORTED)
    {
        ZEROEQWARN << "ZeroEQ::Sender: Cannot announce on Zeroconf; no "
                      "implementation provided by Servus"
                   << std::endl;
        return;
    }
    if (!result)
        ZEROEQTHROW(std::runtime_error("Zeroconf announce failed: " +
                                       result.getString()));
}

void Sender::addSockets(std::vector<zeroeq::detail::Socket>& entries)
{
    zeroeq::detail::Socket entry;
    entry.socket = socket.get();
    entry.events = ZMQ_POLLIN;
    entries.push_back(entry);
}

void Sender::_getEndPoint(std::string& host, std::string& port) const
{
    char buffer[1024];
    size_t size = sizeof(buffer);
    if (zmq_getsockopt(socket.get(), ZMQ_LAST_ENDPOINT, &buffer, &size) == -1)
    {
        ZEROEQTHROW(std::runtime_error("Cannot determine port of publisher"));
    }
    const std::string endPoint(buffer);

    port = endPoint.substr(endPoint.find_last_of(":") + 1);
    const size_t start = endPoint.find_last_of("/") + 1;
    const size_t end = endPoint.find_last_of(":");
    host = endPoint.substr(start, end - start);
    if (host == "0.0.0.0")
    {
        char hostname[NI_MAXHOST + 1] = {0};
        gethostname(hostname, NI_MAXHOST);
        hostname[NI_MAXHOST] = '\0';
        host = hostname;
    }
}

uint128_t& Sender::getUUID()
{
    static uint128_t identifier = servus::make_UUID();
    return identifier;
}
}
}
