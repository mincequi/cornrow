
/* Copyright (c) 2017, Human Brain Project
 *                     Stefan.Eilemann@epfl.ch
 */

#pragma once

#include <zeroeq/receiver.h> // base class

namespace zeroeq
{
/**
 * Requests a remote procedure call on a Server.
 *
 * If the client is in the same session as discovered servers, it
 * automatically connects to those servers. Requests are executed round-robin on
 * all connected servers.
 *
 * A connection to a non-existing server is valid. Requests will be executed
 * once the servers are available.
 *
 * A receive on any instance of a shared group will receive on all instances
 * and call the registered handlers.
 *
 * The session of a Client-Server setup is typically semantically different from
 * the session of a Publisher-Subscriber setup: In the former, it identifies a
 * service provided, i.e., the type of requests served. In the latter, it
 * identifies an interest scope, e.g., all the applications of a given user.
 *
 * Not thread safe.
 *
 * Example: @include tests/reqRep.cpp
 */
class Client : public Receiver
{
public:
    /**
     * Create a default client.
     *
     * Postconditions:
     * - connects to all servers set in the comma-separated environment variable
     *   ZEROEQ_SERVERS
     * - discovers servers on _zeroeq_rep._tcp ZeroConf service
     * - filters session \<username\> or ZEROEQ_SERVER_SESSION from environment
     *
     * @throw std::runtime_error if ZeroConf is not available
     */
    ZEROEQ_API Client();

    /**
     * Create a client which connects to servers from the given session.
     *
     * Postconditions:
     * - connects to all servers set in the comma-separated environment variable
     *   ZEROEQ_SERVERS
     * - discovers publishers on _zeroeq_rep._tcp ZeroConf service
     * - filters for given session
     *
     * @param session session name used for filtering of discovered publishers
     * @throw std::runtime_error if ZeroConf is not available
     */
    ZEROEQ_API explicit Client(const std::string& session);

    /**
     * Create a client which subscribes to specific servers.
     *
     * Postconditions:
     * - connected to the servers on the given URIs once servers are running
     *
     * @param uris publisher URIs in the format *|host|IP|IF:port
     * @throw std::runtime_error if URI is not fully qualified
     */
    ZEROEQ_API explicit Client(const URIs& uris);

    /**
     * Create a default shared client.
     *
     * @sa Client()
     * @param shared another receiver to share data reception with
     */
    ZEROEQ_API explicit Client(Receiver& shared);

    /**
     * Create a shared client which subscribes to servers from the given
     * session.
     *
     * @sa Client( const std::string& )
     *
     * @param session only subscribe to servers of the same session
     * @param shared another receiver to share data reception with
     */
    ZEROEQ_API Client(const std::string& session, Receiver& shared);

    /**
     * Create a shared client which subscribes to servers on the given URIs.
     *
     * @sa Client( const URIs& )
     *
     * @param uris server URIs in the format *|host|IP|IF:port
     * @param shared another receiver to share data reception with
     */
    ZEROEQ_API Client(const URIs& uris, Receiver& shared);

    /** Destroy this client. */
    ZEROEQ_API ~Client();

    /**
     * Request the execution of the given data on a connected Server.
     *
     * The reply function will be executed during receive(). May block when
     * all servers are overloaded or no server is connected.
     *
     * The reply function will get called with (0, nullptr, 0) if the server
     * does not have a handler for the request or if the handler had an
     * exception.
     *
     * @param request the request identifier and payload
     * @param func the function to execute for the reply
     * @return true if the request was sent, false on error
     */
    ZEROEQ_API bool request(const servus::Serializable& request,
                            const ReplyFunc& func);

    /**
     * Request the execution of the given data on a connected Server.
     *
     * See request() overload above for details.
     *
     * @param request the request identifier
     * @param data the payload data of the request, may be nullptr
     * @param size the size of the payload data, may be 0
     * @param func the function to execute for the reply
     */
    ZEROEQ_API bool request(const uint128_t& request, const void* data,
                            size_t size, const ReplyFunc& func);

    /** @return the session name that is used for filtering. */
    ZEROEQ_API const std::string& getSession() const;

private:
    class Impl;
    std::unique_ptr<Impl> _impl;

    // Receiver API
    void addSockets(std::vector<detail::Socket>& entries) final;
    bool process(detail::Socket& socket) final;
    void update() final;
    void addConnection(const std::string& uri) final;
};
}
