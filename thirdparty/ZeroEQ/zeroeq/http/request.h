/* Copyright (c) 2017, EPFL/Blue Brain Project
 *                     Stefan.Eilemann@epfl.ch
 *                     Raphael.Dumusc@epfl.ch
 */

#pragma once

#include <zeroeq/types.h>

#include <string> // member

namespace zeroeq
{
namespace http
{
/** HTTP method used in a Request. */
enum class Method
{
    GET,
    POST,
    PUT,
    PATCH,
    DELETE,
    OPTIONS,
    ALL //!< @internal, must be last
};

/**
 * HTTP Request with method, path and body.
 *
 * The path provides the url part after the registered endpoint if it is
 * terminated with a slash.
 * Registered endpoint || HTTP request         || path
 * "api/windows/"      || "api/windows/jf321f" || "jf321f".
 * "api/windows/"      || "api/windows/"       || ""
 *
 * If an endpoint is not terminated with a slash, then only exactly matching
 * HTTP request will be processed.
 * Registered endpoint || HTTP request          || path
 * "api/objects"       || "api/objects"         || ""
 * "api/objects"       || "api/objects/abc"     || ** ENDPOINT NOT FOUND: 404 **
 *
 * The query is the url part after "?".
 * Registered endpoint || HTTP request                 || query    || path
 * "api/objects"       || "api/objects?size=4"         || "size=4" || ""
 * "api/windows/"      || "api/windows/jf321f?size=4"  || "size=4" || "jf321"
 *
 * The body is the HTTP request payload.
 */
struct Request
{
    Method method;
    std::string path;
    std::string source;
    std::string query;
    std::string body;
};
}
}
