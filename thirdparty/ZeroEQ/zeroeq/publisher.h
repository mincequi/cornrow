
/* Copyright (c) 2014-2017, Human Brain Project
 *                          Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *                          Stefan.Eilemann@epfl.ch
 */

#ifndef ZEROEQ_PUBLISHER_H
#define ZEROEQ_PUBLISHER_H

#include <zeroeq/api.h>
#include <zeroeq/sender.h> // base class
#include <zeroeq/types.h>

#include <memory>

namespace zeroeq
{
/**
 * Serves and publishes events, consumed by Subscriber.
 *
 * The session is tied to ZeroConf announcement and can be disabled by passing
 * zeroeq::NULL_SESSION as the session name.
 *
 * Example: @include tests/publisher.cpp
 */
class Publisher : public Sender
{
public:
    /**
     * Create a default publisher.
     *
     * Postconditions:
     * - bound to all network interfaces
     * - runs on a random port
     * - announces itself on the _zeroeq_pub._tcp ZeroConf service as host:port
     * - announces session \<username\> or ZEROEQ_PUB_SESSION from environment
     *
     * @throw std::runtime_error if session is empty or socket setup fails
     */
    ZEROEQ_API Publisher();

    /**
     * Create a publisher which announces the specified session.
     *
     * Postconditions:
     * - bound to all network interfaces
     * - runs on a random port
     * - announces itself on the _zeroeq_pub._tcp ZeroConf service as host:port
     * - announces given session
     *
     * @param session session name used for announcement
     * @throw std::runtime_error if session is empty or socket setup fails
     */
    ZEROEQ_API explicit Publisher(const std::string& session);

    /**
     * Create a publisher which runs on the specified URI.
     *
     * Postconditions:
     * - bound to the host and/or port from the given URI
     * - announces itself on the _zeroeq_pub._tcp ZeroConf service as host:port
     * - announces session \<username\> or ZEROEQ_PUB_SESSION from environment
     *
     * @param uri publishing URI in the format [scheme://][*|host|IP|IF][:port]
     * @throw std::runtime_error if session is empty or socket setup fails
     */
    ZEROEQ_API explicit Publisher(const URI& uri);

    /**
     * Create a publisher which runs on the specified URI and announces the
     * specified session.
     *
     * Postconditions:
     * - bound to the host and/or port from the given URI
     * - announces itself on the _zeroeq_pub._tcp ZeroConf service as host:port
     * - announces given session
     *
     * @param session session name used for announcement
     * @param uri publishing URI in the format [scheme://][*|host|IP|IF][:port]
     * @throw std::runtime_error if session is empty or socket setup fails
     */
    ZEROEQ_API Publisher(const URI& uri, const std::string& session);

    ZEROEQ_API ~Publisher();

    /**
     * Publish the given serializable object to any subscriber.
     *
     * If there is no subscriber for that serializable, no message will be sent.
     *
     * @param serializable the object to publish
     * @return true if publish was successful
     */
    ZEROEQ_API bool publish(const servus::Serializable& serializable);

    /**
     * Publish the given event to any subscriber.
     *
     * If there is no subscriber for that event, no message will be sent.
     *
     * @param event the event identifier to publish
     * @return true if publish was successful
     */
    ZEROEQ_API bool publish(const uint128_t& event);

    /**
     * Publish the given event with payload to any subscriber.
     *
     * If there is no subscriber for that event, no message will be sent.
     *
     * @param event the event identifier to publish
     * @param data the payload data of the event
     * @param size the size of the payload data
     * @return true if publish was successful
     */
    ZEROEQ_API bool publish(const uint128_t& event, const void* data,
                            size_t size);

    /**
     * Get the publisher URI.
     *
     * Contains the used hostname and port, if none where given in the
     * constructor uri.
     *
     * @return the publisher URI.
     */
    ZEROEQ_API const URI& getURI() const;

    /** @return the session name that is announced */
    ZEROEQ_API const std::string& getSession() const;

    ZEROEQ_API std::string getAddress() const; //!< @internal

private:
    class Impl;
    std::unique_ptr<Impl> _impl;

    Publisher(const Publisher&) = delete;
    Publisher& operator=(const Publisher&) = delete;

    ZEROEQ_API zmq::SocketPtr getSocket() final;
};
}

#endif
