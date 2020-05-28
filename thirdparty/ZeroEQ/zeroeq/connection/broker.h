
/* Copyright (c) 2014-2017, Human Brain Project
 *                          Stefan.Eilemann@epfl.ch
 */

#ifndef ZEROEQ_CONNECTION_BROKER_H
#define ZEROEQ_CONNECTION_BROKER_H

#include <zeroeq/log.h>
#include <zeroeq/receiver.h> // base class

namespace zeroeq
{
/**
 * Connection broker to introduce remote publishers to a subscriber.
 *
 * The subscriber side sets up a Broker which listens on a well-defined
 * port. The zeroeq::Publisher side uses the Service to subscribe the remote
 * zeroeq::Subscriber to the publisher in the absence of zeroconf, e.g., when
 * connecting applications in different subnets. Named receivers are a
 * convenience to select the port based on an application name(space). Only one
 * application can run on a given machine.
 *
 * Example: @include tests/connection/broker.cpp
 */
namespace connection
{
namespace detail
{
class Broker;
}

/**
 * Brokers subscription requests for a zeroeq::Receiver.
 *
 * Example: @include tests/connection/broker.cpp
 */
class Broker : public Receiver
{
public:
    enum PortSelection
    {
        PORT_FIXED,          //!< Use only the fixed port for named brokers
        PORT_FIXED_OR_RANDOM //!< Fall back to a random port on failure.
    };

    /**
     * Convenience constructor to create a new subscription broker.
     *
     * This constructor will try to bind to INADDR_ANY and a fixed port derived
     * from the given name. If that fails, it will throw a std::runtime_error
     * (if mode is PORT_FIXED) or allocate a random port (if mode is
     * PORT_FIXED_OR_RANDOM).
     *
     * The name should be a string of the application's namespace, e.g.,
     * "livre". The same string should be used by Service::subscribe(). A
     * hashing algorithm is used to derive the port from the name. This yields a
     * fixed port number, therefore only a single application per machine can
     * run one broker. If the random port fallback is enabled, this constructor
     * will allocate a random available port.
     *
     * @param name the application namespace.
     * @param receiver the Receiver to manage.
     * @param mode the allocation strategy if the fixed port is unavailable.
     * @throw std::runtime_error when the zmq setup failed.
     */
    ZEROEQ_API Broker(const std::string& name, Receiver& receiver,
                      const PortSelection mode);

    /**
     * Create a new subscription broker.
     *
     * The given receiver has to have at least the same lifetime as this
     * broker. The receiver and broker are automatically shared.
     *
     * For simplicity, only a single Receiver is handled by a Broker. The
     * implementation should be extended if multiple receivers shall be
     * handled.
     *
     * @param address the zmq reply socket address to be used.
     * @param receiver the Receiver to manage.
     * @throw std::runtime_error when the zmq setup failed.
     */
    ZEROEQ_API Broker(const std::string& address, Receiver& receiver);

    /** Destroy this broker. */
    ZEROEQ_API ~Broker();

    ZEROEQ_API std::string getAddress() const; //!< @internal

private:
    detail::Broker* const _impl;

    // Receiver API
    void addSockets(std::vector<zeroeq::detail::Socket>& entries) final;
    bool process(zeroeq::detail::Socket& socket) final;
};
}
}
#endif
