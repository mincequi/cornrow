
/* Copyright (c) 2015-2017, Human Brain Project
 *                          Stefan.Eilemann@epfl.ch
 */

#pragma once

#include <zeroeq/detail/context.h>
#include <zeroeq/types.h>
#include <zeroeq/uri.h> // member

#include <servus/servus.h> // member

namespace zeroeq
{
namespace detail
{
class Sender
{
    zmq::ContextPtr _context; // must be private before socket

public:
    ZEROEQ_API Sender(const URI& uri_, const int type);
    ZEROEQ_API Sender(const URI& uri_, const int type,
                      const std::string service, const std::string& session);
    ZEROEQ_API ~Sender();

    std::string getAddress() const;

    void initURI();
    ZEROEQ_API void announce();
    void addSockets(std::vector<zeroeq::detail::Socket>& entries);

    const std::string& getSession() const { return _session; }
    ZEROEQ_API static uint128_t& getUUID();

    URI uri;
    zmq::SocketPtr socket;

private:
    void _getEndPoint(std::string& host, std::string& port) const;
    void* _createContext(void* context);

    servus::Servus _service;
    const std::string _session;
};
}
}
