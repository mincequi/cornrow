HTTP Server for generic REST interfaces {#httpserverrest}
============

This document describes a proposed extension to the current http::Server to make
it usable as a general-purpose REST interface.

## Requirements

* Support all standard http verbs: GET, POST, PUT, PATCH, DELETE
* Support custom response payload for all action verbs (not just for GET)
* Allow specifying the MIME type of the response to serve any type of content
* Allow specifying extra HTTP header fields in response to certain requests
* Allow asynchronous processing of requests (not blocking the main thread)
* Single endpoint for collections (e.g. windows/, files/, neurons/, etc. )
* Allow filtering operations on collections (windows/selected, files?limit=10)

## Dependency Changes

None, cppnetlib 0.13 already offers the required functionality.

# Proposition #1

## API

To fulfill the requirements with minimal API changes, we propose a generic
interface as follows:

    namespace zeroeq
    {
    namespace http
    {
    enum class Verb { GET, POST, PUT, PATCH, DELETE };

    // copied from HTTPServer::connection::status_t
    enum Code { OK = 200, NOT_FOUND = 404, ... };

    // some commonly used headers, see:
    // https://en.wikipedia.org/wiki/List_of_HTTP_header_fields
    enum class Header{ LOCATION, RETRY_AFTER, ... };

    struct Response
    {
        Code code = Code::OK;
        std::string contentType;
        std::string payload;
        std::map<Header, std::string> headers;
    };

    /** HTTP REST callback with payload, returning a Response future. */
    using RESTFunc = std::function<std::future<Response>(const std::string&)>;

    /** HTTP REST callback with path + payload, returning a Response future. */
    using RESTPathFunc = std::function<std::future<Response>(const std::string&,
                                                             const std::string&)>;

    class Server
    {
    public:
        /** ... */

        /** Handle a single action on a given endpoint. */
        void handle(Verb action, const std::string& endpoint, RESTFunc func);

        /** Handle all urls starting with the given endpoint. */
        void handlePath(Verb action, const std::string& endpoint,
                        RESTPathFunc func);

        /** ... */
    }
    }
    }


## Examples

The following example shows how the API could be use to expose a collection of
windows. For illustration purposes, both handle() and handlePath() are used.
Note how the asychronous API is used to process the most time consuming requests
without blocking the main thread.

    using namespace zeroeq;
    http::Server server{ /** ... */ };

    void makeThumbnail(const std::string& windowId)
    {
        auto response = http::Response{};
        response.contentType = "image/jpeg";
        response.payload = doTheHeavyWork(windowId);
        return response;
    }

    // simplified window collection for the example
    std::set<std::string> windows{"1234", "567"};

    // Register an individual endpoint for each thumbnail. Note that this is
    // tedious to manage as the list of windows evolves. It could be more
    // efficiently served by a generic function:
    // server.handlePath(http::Verb::GET, "api/windows", getWindowInfo );
    for (const auto& windowId : windows)
    {
        const auto makeThumbnailAsync = [windowId](const std::string&)
        {
            return std::async(std::launch::async, makeThumbnail, windowId);
        };
        server.handle(http::Verb::GET, "api/windows/"+windowId+"/thumbnail",
                      makeThumbnailAsync);
    }

    // Here we use a generic endpoint for all window close operations. "path"
    // provides the url part after the endpoint: "api/windows/567" -> "567"
    const auto closeWindow = [&](const std::string& path, const std::string&)
    {
        if (!windows.count(path))
            return make_ready_future(http::Response{http::Code::NOT_FOUND});

        server.remove("api/windows/"+path+"/thumbnail");
        windows.erase(path);
        return make_ready_future(http::Response{http::Code::OK});
    };
    server.handlePath(http::Verb::DELETE, "api/windows", closeWindow);

    // This function uses POST to open a new window and return its location.
    const auto openWindow = [&](const std::string& request)
    {
        if (!windows.insert(request))
            return make_ready_future(http::Response{http::Code::NO_CONTENT});

        auto response = http::Response();
        response.code = http::Code::CREATED;
        response.headers[http::Header::LOCATION] = "api/windows/"+request;
        return make_ready_future(std::move(response));
    };
    server.handle(http::Verb::POST, "api/windows", openWindow);

## Implementation

In requestHandler.h, the struct HTTPRequest needs to be changed from:

    // output from zeroeq::http::Server
    HTTPServer::connection::status_t status;
    std::string reply;

to

    // output from zeroeq::http::Server
    std::future<Response> response;

In requestHandler.cpp, the ConnectionHandler must wait on response.get();

In server.cpp, all existing handle*() functions must be adapted to wrap the
response data as ready futures. Additional maps must be added for the POST,
PATCH and DELETE actions.

# Proposition #2

This proposition makes use of cppnetlib's internal thread pool to execute the
asynchronous functions, removing some complexity from user code.

## API

Same enums and Response struct as in proposition #1:

    namespace zeroeq
    {
    namespace http
    {

    /** HTTP REST callback with payload, returning an http Response. */
    using RESTFunc = std::function<Response(const std::string&)>;

    /** Returned by a RESTPathFunc. Stores the actual RESTFunc to execute
        and a boolean indicating if it must be run asynchronously. */
    struct PathResponse
    {
        RESTFunc func;
        bool async = false;
    };

    /** HTTP REST callback with path + payload, returning a PathResponse.
        "path" provides the url part after the endpoint, for instance:
        endpoint: "api/windows" | url: "api/windows/567/thumbnail" ->
        path: "567/thumbnail"
    */
    using RESTPathFunc = std::function<PathResponse(const std::string&,
                                                    const std::string&)>;

    class Server
    {
    public:
        /** ... */

        /** Handle a single action on a given endpoint. */
        void handle(Verb action, const std::string& endpoint, RESTFunc func,
                    bool async);

        /** Handle all urls starting with the given endpoint. */
        void handlePath(Verb action, const std::string& endpoint,
                        RESTPathFunc func);

        /** ... */
    }
    }
    }


## Examples

This example is similar to the one in proposition #1. It explores the handling
of path requests in a synchronous or asynchronous manner depending on the url.

    using namespace zeroeq;
    http::Server server{ /** ... */ };

    // simplified window collection for the example
    std::set<std::string> windows{"1234", "567"};

    // example path: "", "567/thumbnail", "567/wrongproperty"
    const auto getWindowInfo = [&](const std::string& path, const std::string&)
    {
        // GET "api/windows" -> list all windows
        if (path.empty())
        {
            const auto func = [&](const std::string&)
            {
                auto response = http::Response;
                response.contentType = "application/json";
                response.payload = toJsonString(windows);
                return response;
            }
            return PathResponse{func, false};
        }

        const auto input = split(path,'/');
        const auto& windowId = input[0];
        const auto& property = input.size() == 2 ? input[1] : std::string();

        if (!windows.count(windowId))
        {
            return PathResponse{[](const std::string&)
                      { return http::Response{http::Code::NOT_FOUND}; }, false};
        }
        if (property == "thumbnail")
        {
            const auto makeThumbnail = [windowId](const std::string&)
            {
                auto response = http::Response{};
                response.contentType = "image/jpeg";
                response.payload = doTheHeavyWork(windowId);
                return response;
            };
            return PathResponse{makeThumbnail, true};
        }
        return PathResponse{[](const std::string&)
                { return http::Response{http::Code::NOT_IMPLEMENTED}; }, false};
    }
    server.handlePath(http::Verb::GET, "api/windows", getWindowInfo);

    // Here we use a generic endpoint for window close operations. "path"
    // contains the url part after the endpoint: "api/windows/567" -> "567"
    const auto closeWindow = [&](const std::string& path)
    {
        if (!windows.count(path))
            return http::Response{http::Code::NOT_FOUND};

        server.remove("api/windows/"+path+"/thumbnail");
        windows.erase(path);
        return http::Response{http::Code::OK};
    };
    const auto deleteWindow = [&](const std::string& path, const std::string&)
    {
        const auto func = [path](const std::string&){ closeWindow(path); };
        return PathResponse{func, false};
    };
    server.handlePath(http::Verb::DELETE, "api/windows", deleteWindow);

    // This function uses POST to open a new window and return its location.
    const auto openWindow = [&](const std::string& request)
    {
        if (!windows.insert(request))
            return http::Response{http::Code::NO_CONTENT};

        auto response = http::Response();
        response.code = http::Code::CREATED;
        response.headers[http::Header::LOCATION] = "api/windows/"+request;
        return response;
    };
    server.handle(http::Verb::POST, "api/windows", openWindow, false);

## Implementation

    In requestHandler.h, the struct HTTPRequest needs to be changed from:

        // output from zeroeq::http::Server
        HTTPServer::connection::status_t status;
        std::string reply;

    to

        // output from zeroeq::http::Server
        Response response;
        RESTFunc asyncResponse;

    In requestHandler.cpp, ConnectionHandler::_handleRequest must do:
    if (httpRequest.asyncResponse)
        response = httpRequest.asyncResponse();

    In server.cpp, additional maps must be added for the POST, PATCH and DELETE
    actions. Each map must also store a boolean indicating whether the function
    should be called synchronusly or asynchronously.
    Adding support for handlePath() should be straighforward.

## Issues

### 1: How many threads should the cppnetlib server use?

_Resolution: Open:_ The server thread pool consists of a single thread by
default. Using more threads may improve performance by accepting multiple
concurrent requests but would require changes in the zeroeq inproc signaling
mechanism.
