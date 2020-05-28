
/* Copyright (c) 2017, Human Brain Project
 *                          Stefan.Eilemann@epfl.ch
 */

#pragma once

#include <zeroeq/types.h>

namespace zeroeq
{
/** Interface for entities sending data. */
class Sender
{
public:
    virtual ~Sender() {}
    virtual zmq::SocketPtr getSocket() = 0; //!< @return the ZMQ socket
};
}
