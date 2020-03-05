
/* Copyright (c) 2017, Human Brain Project
 *                     Stefan.Eilemann@epfl.ch
 */

#include "context.h"

#include <zmq.h>

#include <mutex>

namespace zeroeq
{
namespace detail
{
zmq::ContextPtr getContext()
{
    static std::weak_ptr<void> context;
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);

    zmq::ContextPtr sharedContext = context.lock();
    if (!sharedContext)
    {
        sharedContext.reset(zmq_ctx_new(),
                            [](void* c) { ::zmq_ctx_destroy(c); });
        context = sharedContext;
    }
    return sharedContext;
}
}
}
