
/* Copyright (c) 2014-2017, Human Brain Project
 *                          Stefan.Eilemann@epfl.ch
 */

#ifndef ZEROEQ_RECEIVER_H
#define ZEROEQ_RECEIVER_H

#include <zeroeq/api.h>
#include <zeroeq/types.h>

#include <memory>

#include <string>
#include <vector>

namespace zeroeq
{
namespace connection
{
namespace detail
{
class Broker;
}
}

/**
 * Base class for entities receiving data.
 *
 * Provides a receive() method, which demultiplexes data between multiple inputs
 * of multiple instances of receivers. Receivers form a shared group by linking
 * them at construction time.
 *
 * Not intended to be as a final class. Not thread safe.
 *
 * Example: @include tests/receiver.cpp
 */
class Receiver
{
public:
    /** Create a new standalone receiver. */
    ZEROEQ_API Receiver();

    /**
     * Create a shared receiver.
     *
     * All receivers sharing a group may receive data when receive() is called
     * on any of them.
     *
     * @param shared another receiver to form a simultaneous receive group with.
     */
    ZEROEQ_API explicit Receiver(Receiver& shared);

    ZEROEQ_API virtual ~Receiver();
    ZEROEQ_API Receiver(Receiver&&);
    ZEROEQ_API Receiver& operator=(Receiver&&);

    /**
     * Receive at least one event from all shared receivers.
     *
     * Using receive( 0 ) is equivalent to polling the receivers for data.
     *
     * @param timeout timeout in ms for poll, default blocking poll until at
     *                least one event is received
     * @return true if at least one event was received
     * @throw std::runtime_error when polling failed.
     */
    ZEROEQ_API bool receive(const uint32_t timeout = TIMEOUT_INDEFINITE);

protected:
    /** Add this receiver's sockets to the given list */
    virtual void addSockets(std::vector<detail::Socket>& entries) = 0;

    /**
     * Process data on a signalled socket.
     *
     * @param socket the socket provided from addSockets()
     * @return true if an event was communicated to the application, false
     *         otherwise
     */
    virtual bool process(detail::Socket& socket) = 0;

    /**
     * Update the internal connection list.
     *
     * Called on all members of a shared group regularly by receive() to update
     * their list of sockets.
     */
    virtual void update() {}

    /**
     * Add the given connection to the list of receiving sockets.
     *
     * @param uri the ZeroMQ address to connect to.
     */
    ZEROEQ_API virtual void addConnection(const std::string& uri);
    friend class connection::detail::Broker;

private:
    Receiver& operator=(const Receiver&) = delete;

    class Impl;
    std::shared_ptr<Impl> _impl;
};
}

#endif
