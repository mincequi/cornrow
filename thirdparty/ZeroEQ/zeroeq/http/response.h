/* Copyright (c) 2017, EPFL/Blue Brain Project
 *                     Pawel Podhajski <pawel.podhajski@epfl.ch>
 */

#ifndef ZEROEQ_HTTP_RESPONSE_H
#define ZEROEQ_HTTP_RESPONSE_H

#include <map>    // member
#include <string> // member

namespace zeroeq
{
namespace http
{
/** HTTP headers which can be used in a Response. */
enum class Header
{
    ALLOW,
    CONTENT_TYPE,
    LAST_MODIFIED,
    LOCATION,
    RETRY_AFTER
};

/** HTTP codes to be used in a Response. */
enum Code
{
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,
    PARTIAL_CONTENT = 206,
    MULTIPLE_CHOICES = 300,
    MOVED_PERMANENTLY = 301,
    MOVED_TEMPORARILY = 302,
    NOT_MODIFIED = 304,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    NOT_SUPPORTED = 405,
    NOT_ACCEPTABLE = 406,
    REQUEST_TIMEOUT = 408,
    PRECONDITION_FAILED = 412,
    UNSATISFIABLE_RANGE = 416,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
    SPACE_UNAVAILABLE = 507
};

/**
 * Response to an HTTP Request.
 */
struct Response
{
    /** HTTP return code. */
    Code code;

    /** Payload to return in a format specified in CONTENT_TYPE header. */
    std::string body;

    /** HTTP message headers. */
    std::map<Header, std::string> headers;

    /** Construct a Response with a given return code and payload. */
    Response(const Code code_ = Code::OK,
             const std::string& body_ = std::string())
        : code{code_}
        , body{body_}
    {
    }

    /** Construct a Response with a given code, payload and content type. */
    Response(const Code code_, const std::string& body_,
             const std::string& contentType)
        : code{code_}
        , body{body_}
        , headers{{Header::CONTENT_TYPE, contentType}}
    {
    }

    /** Construct a Response with a given code, payload and map of headers. */
    Response(const Code code_, const std::string& body_,
             std::map<Header, std::string> headers_)
        : code{code_}
        , body{body_}
        , headers{{std::move(headers_)}}
    {
    }
};
}
}

#endif
