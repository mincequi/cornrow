
/* Copyright (c) 2017, Human Brain Project
 *                          Stefan.Eilemann@epfl.ch
 */

#pragma once

#include <zeroeq/receiver.h> // base class

namespace zeroeq
{
/**
 * Monitors a Sender and notifies on events on its socket.
 *
 * @warning: Installing multiple monitors on a single Sender has undefined
 * behaviour.
 */
class Monitor : public Receiver
{
public:
    /** Monitor the given sender. */
    ZEROEQ_API explicit Monitor(Sender& sender);

    /** Monitor the given sender and notify with the given shared group. */
    ZEROEQ_API Monitor(Sender& sender, Receiver& shared);

    /** Destroy this monitor*/
    ZEROEQ_API ~Monitor();

    /** Notify of a new connection to the sender. */
    virtual void notifyNewConnection() {}

    class Impl;

private:
    std::unique_ptr<Impl> _impl;

    Monitor(const Monitor&) = delete;
    Monitor& operator=(const Monitor&) = delete;

    // Receiver API
    ZEROEQ_API void addSockets(std::vector<zeroeq::detail::Socket>& entries) final;
    ZEROEQ_API bool process(zeroeq::detail::Socket& socket) final;
};
}
