
/* Copyright (c) 2017, Human Brain Project
 *                     Stefan.Eilemann@epfl.ch
 */

#pragma once

#include <zeroeq/api.h>
#include <zeroeq/receiver.h> // base class
#include <zeroeq/sender.h>   // base class
#include <zeroeq/types.h>

namespace zeroeq
{
/**
 * Serves request from one or more Client(s).
 *
 * The session is tied to ZeroConf announcement and can be disabled by passing
 * zeroeq::NULL_SESSION as the session name.
 *
 * Example: @include tests/reqRep.cpp
 */
class Server : public Receiver, public Sender
{
public:
    /**
     * Create a default server.
     *
     * Postconditions:
     * - bound to all network interfaces
     * - runs on a random port
     * - announces itself on the _zeroeq_rep._tcp ZeroConf service as host:port
     * - announces session \<username\> or ZEROEQ_SERVER_SESSION from
     * environment
     *
     * @throw std::runtime_error if session is empty or socket setup fails
     */
    ZEROEQ_API Server();

    /**
     * Create a server which announces itself using the specified session.
     *
     * Postconditions:
     * - bound to all network interfaces
     * - runs on a random port
     * - announces itself on the _zeroeq_rep._tcp ZeroConf service as host:port
     * - announces given session
     *
     * @param session session name used for announcement
     * @throw std::runtime_error if session is empty or socket setup fails
     */
    ZEROEQ_API explicit Server(const std::string& session);

    /**
     * Create a server which runs on the specified URI.
     *
     * Postconditions:
     * - bound to the host and/or port from the given URI
     * - announces itself on the _zeroeq_rep._tcp ZeroConf service as host:port
     * - announces session \<username\> or ZEROEQ_SERVER_SESSION from
     * environment
     *
     * @param uri publishing URI in the format [*|host|IP|IF][:port]
     * @throw std::runtime_error if session is empty or socket setup fails
     */
    ZEROEQ_API explicit Server(const URI& uri);

    /**
     * Create a server which runs on the specified URI and announces the
     * specified session.
     *
     * Postconditions:
     * - bound to the host and/or port from the given URI
     * - announces itself on the _zeroeq_rep._tcp ZeroConf service as host:port
     * - announces given session
     *
     * @param session session name used for announcement
     * @param uri publishing URI in the format [*|host|IP|IF][:port]
     * @throw std::runtime_error if session is empty or socket setup fails
     */
    ZEROEQ_API Server(const URI& uri, const std::string& session);

    /**
     * Create a default server.
     *
     * Postconditions:
     * - bound to all network interfaces
     * - runs on a random port
     * - announces itself on the _zeroeq_rep._tcp ZeroConf service as host:port
     * - announces session \<username\> or ZEROEQ_SERVER_SESSION from
     * environment
     *
     * @param shared another receiver to share data reception with
     * @throw std::runtime_error if session is empty or socket setup fails
     */
    ZEROEQ_API explicit Server(Receiver& shared);

    /**
     * Create a server which announces itself using the specified session.
     *
     * Postconditions:
     * - bound to all network interfaces
     * - runs on a random port
     * - announces itself on the _zeroeq_rep._tcp ZeroConf service as host:port
     * - announces given session
     *
     * @param session session name used for announcement
     * @param shared another receiver to share data reception with
     * @throw std::runtime_error if session is empty or socket setup fails
     */
    ZEROEQ_API Server(const std::string& session, Receiver& shared);

    /**
     * Create a server which runs on the specified URI.
     *
     * Postconditions:
     * - bound to the host and/or port from the given URI
     * - announces itself on the _zeroeq_rep._tcp ZeroConf service as host:port
     * - announces session \<username\> or ZEROEQ_SERVER_SESSION from
     * environment
     *
     * @param uri publishing URI in the format [*|host|IP|IF][:port]
     * @param shared another receiver to share data reception with
     * @throw std::runtime_error if session is empty or socket setup fails
     */
    ZEROEQ_API Server(const URI& uri, Receiver& shared);

    /**
     * Create a server which runs on the specified URI and announces the
     * specified session.
     *
     * Postconditions:
     * - bound to the host and/or port from the given URI
     * - announces itself on the _zeroeq_rep._tcp ZeroConf service as host:port
     * - announces given session
     *
     * @param session session name used for announcement
     * @param uri publishing URI in the format [*|host|IP|IF][:port]
     * @param shared another receiver to share data reception with
     * @throw std::runtime_error if session is empty or socket setup fails
     */
    ZEROEQ_API Server(const URI& uri, const std::string& session,
                      Receiver& shared);

    /** Destroy this server. */
    ZEROEQ_API ~Server();

    ZEROEQ_API Server(Server&&);
    ZEROEQ_API Server& operator=(Server&&);

    /**
     * Register a request handler.
     *
     * Exceptions in a request handler are considered an error (0 is returned to
     * client).
     *
     * @param request the request to handle
     * @param func the function to call on receive() of a Client::request()
     * @return true if subscription was successful, false otherwise
     */
    ZEROEQ_API bool handle(const uint128_t& request, const HandleFunc& func);

    /**
     * Remove a registered request handler.
     *
     * @return true if the handler was removed, false if it was not registered.
     */
    ZEROEQ_API bool remove(const uint128_t& request);

    /**
     * Get the server URI.
     *
     * Contains the used hostname and port, if none where given in the
     * constructor uri.
     *
     * @return the server URI.
     */
    ZEROEQ_API const URI& getURI() const;

    /** @return the session name that is announced */
    ZEROEQ_API const std::string& getSession() const;

private:
    class Impl;
    std::unique_ptr<Impl> _impl;

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    // Receiver API
    void addSockets(std::vector<detail::Socket>& entries) final;
    bool process(detail::Socket& socket) final;
    void addConnection(const std::string& uri) final;

    // Sender API
    ZEROEQ_API zmq::SocketPtr getSocket() final;
};
}
