
/* Copyright (c) 2017, Human Brain Project
 *                     Stefan.Eilemann@epfl.ch
 */

#pragma once

#include <zeroeq/types.h>

#include <functional>
#include <future>

namespace zeroeq
{
/** HTTP protocol support.
 *
 * The http::Server is a zeroeq::Receiver which receives and handles http
 * requests.
 */
namespace http
{
class Server;
struct Request;
struct Response;

/** HTTP PUT callback w/o payload, return reply success. */
using PUTFunc = std::function<bool()>;

/** HTTP PUT callback w/ JSON payload, return reply success. */
using PUTPayloadFunc = std::function<bool(const std::string&)>;

/** HTTP GET callback to return JSON reply. */
using GETFunc = std::function<std::string()>;

/** HTTP REST callback with Request parameter returning a Response future. */
using RESTFunc = std::function<std::future<Response>(const Request&)>;
}
}
