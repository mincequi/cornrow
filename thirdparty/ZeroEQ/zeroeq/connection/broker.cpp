
/* Copyright (c) 2014-2017, Human Brain Project
 *                          Stefan.Eilemann@epfl.ch
 */

#include "broker.h"

#include <zeroeq/detail/port.h>
#include <zeroeq/detail/sender.h>
#include <zeroeq/detail/socket.h>
#include <zeroeq/log.h>
#include <zeroeq/receiver.h>

#include <cassert>
#include <map>

namespace zeroeq
{
namespace connection
{
namespace detail
{
class Broker : public zeroeq::detail::Sender
{
public:
    Broker(const std::string& name, Receiver& receiver,
           const connection::Broker::PortSelection mode)
        : Sender(URI(std::string("tcp://*:") +
                     std::to_string(uint32_t(zeroeq::detail::getPort(name)))),
                 ZMQ_REP)
        , _receiver(receiver)
    {
        if (!_listen(mode))
        {
            uri = URI("tcp://*:0");
            _listen(connection::Broker::PORT_FIXED);
        }
        initURI();
    }

    Broker(Receiver& receiver, const std::string& address)
        : Sender(URI(std::string("tcp://") + address), ZMQ_REP)
        , _receiver(receiver)
    {
        _listen(connection::Broker::PORT_FIXED);
        initURI();
    }

    ~Broker() {}
    void addSockets(std::vector<zeroeq::detail::Socket>& entries)
    {
        assert(socket);
        if (!socket)
            return;

        zeroeq::detail::Socket entry;
        entry.socket = socket.get();
        entry.events = ZMQ_POLLIN;
        entries.push_back(entry);
    }

    bool process(zeroeq::detail::Socket& socket_)
    {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        zmq_msg_recv(&msg, socket_.socket, 0);
        const std::string address((const char*)zmq_msg_data(&msg),
                                  zmq_msg_size(&msg));

        _receiver.addConnection(std::string("tcp://") + address);
        zmq_msg_send(&msg, socket_.socket, 0);
        zmq_msg_close(&msg);
        return true;
    }

private:
    zeroeq::Receiver& _receiver;

    bool _listen(const connection::Broker::PortSelection mode)
    {
        const std::string address =
            std::to_string(uri) + (uri.getPort() ? "" : ":0");
        if (zmq_bind(socket.get(), address.c_str()) == -1)
        {
            if (mode == connection::Broker::PORT_FIXED)
                ZEROEQTHROW(std::runtime_error("Cannot connect broker to " +
                                               address + ": " +
                                               zmq_strerror(zmq_errno())));

            return false;
        }

        ZEROEQINFO << "Bound broker to " << address << std::endl;
        return true;
    }
};
}

Broker::Broker(const std::string& name, Receiver& receiver,
               const PortSelection mode)
    : Receiver(receiver)
    , _impl(new detail::Broker(name, receiver, mode))
{
}

Broker::Broker(const std::string& address, Receiver& receiver)
    : Receiver(receiver)
    , _impl(new detail::Broker(receiver, address))
{
}

Broker::~Broker()
{
    delete _impl;
}

void Broker::addSockets(std::vector<zeroeq::detail::Socket>& entries)
{
    _impl->addSockets(entries);
}

bool Broker::process(zeroeq::detail::Socket& socket)
{
    return _impl->process(socket);
}

std::string Broker::getAddress() const
{
    return _impl->getAddress();
}
}
}
