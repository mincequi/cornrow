
/* Copyright (c) 2017, Human Brain Project
 *                     Stefan.Eilemann@epfl.ch
 */

#ifndef ZEROEQ_HTTP_HELPERS_H
#define ZEROEQ_HTTP_HELPERS_H

#include <zeroeq/http/response.h>

#include <future>

namespace zeroeq
{
namespace http
{
/**
 * @return ready future wrapping an HTTP Response constructed with
 *         the values passed.
 */
template <typename... Args>
std::future<Response> make_ready_response(Args&&... args)
{
    std::promise<Response> promise;
    promise.set_value(Response(std::forward<Args>(args)...));
    return promise.get_future();
}
}
}

#endif
