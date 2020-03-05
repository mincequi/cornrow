
/* Copyright (c) 2014-2017, Human Brain Project
 *                          Stefan.Eilemann@epfl.ch
 */

#include "service.h"
#include <zeroeq/detail/context.h>
#include <zeroeq/detail/port.h>
#include <zeroeq/log.h>
#include <zeroeq/publisher.h>

#include <zmq.h>

#include <string.h>

namespace zeroeq
{
namespace connection
{
bool Service::subscribe(const std::string& address, const Publisher& publisher)
{
    zmq::ContextPtr context = detail::getContext();
    void* socket = zmq_socket(context.get(), ZMQ_REQ);
    if (!socket)
    {
        ZEROEQINFO << "Can't create socket: " << zmq_strerror(zmq_errno())
                   << std::endl;
        return false;
    }

    const std::string zmqAddress = std::string("tcp://") + address;
    if (zmq_connect(socket, zmqAddress.c_str()) == -1)
    {
        ZEROEQINFO << "Can't reach connection broker at " << address
                   << std::endl;
        zmq_close(socket);
        return false;
    }

    const std::string& pubAddress = publisher.getAddress();
    zmq_msg_t request;
    zmq_msg_init_size(&request, pubAddress.size());
    memcpy(zmq_msg_data(&request), pubAddress.c_str(), pubAddress.size());

    if (zmq_msg_send(&request, socket, 0) == -1)
    {
        zmq_msg_close(&request);
        ZEROEQINFO << "Can't send connection request " << pubAddress << " to "
                   << address << ": " << zmq_strerror(zmq_errno()) << std::endl;
        return false;
    }
    zmq_msg_close(&request);

    zmq_msg_t reply;
    zmq_msg_init(&reply);
    if (zmq_msg_recv(&reply, socket, 0) == -1)
    {
        zmq_msg_close(&reply);
        ZEROEQINFO << "Can't receive connection reply from " << address
                   << std::endl;
        return false;
    }

    const std::string result((const char*)zmq_msg_data(&reply),
                             zmq_msg_size(&reply));
    zmq_msg_close(&reply);

    zmq_close(socket);

    return pubAddress == std::string(result);
}

bool Service::subscribe(const std::string& hostname, const std::string& name,
                        const Publisher& publisher)
{
    const std::string address(hostname + ":" +
                              std::to_string(uint32_t(detail::getPort(name))));
    return subscribe(address, publisher);
}
}
}
