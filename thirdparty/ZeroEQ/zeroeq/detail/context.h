
/* Copyright (c) 2017, Human Brain Project
 *                     Stefan.Eilemann@epfl.ch
 */

#pragma once

#include <zeroeq/api.h>
#include <zeroeq/types.h>

#include <memory>

namespace zeroeq
{
namespace detail
{
/** @return the current ZeroMQ context. Users need to hold onto this context to
 *          extend its lifetime to all sockets created from the context.
 */
ZEROEQ_API zmq::ContextPtr getContext();
}
}
