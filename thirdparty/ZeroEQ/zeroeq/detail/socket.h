
/* Copyright (c) 2014, Human Brain Project
 *                     Stefan.Eilemann@epfl.ch
 */

#ifndef ZEROEQ_DETAIL_SOCKET_H
#define ZEROEQ_DETAIL_SOCKET_H

#include <zmq.h>

namespace zeroeq
{
namespace detail
{
/**
 * Wrapper to hide zmq_pollitem_t from the API (it's a typedef which can't be
 * forward declared)
 */
struct Socket : public zmq_pollitem_t
{
    Socket& operator=(const zmq_pollitem_t& from)
    {
        if (this != &from)
            *static_cast<zmq_pollitem_t*>(this) = from;
        return *this;
    }
};
}
}

#endif
