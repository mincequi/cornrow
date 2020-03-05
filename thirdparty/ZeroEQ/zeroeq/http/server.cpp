
/* Copyright (c) 2016-2017, Human Brain Project
 *                          Stefan.Eilemann@epfl.ch
 *                          Daniel.Nachbaur@epfl.ch
 *                          Pawel.Podhajski@epfl.ch
 *                          Raphael.Dumusc@epfl.ch
 */

#include "server.h"

#include "helpers.h"
#include "requestHandler.h"

#include "../detail/common.h"
#include "../detail/sender.h"
#include "../detail/socket.h"
#include "../log.h"

#include "jsoncpp/json/json.h"

#include <servus/serializable.h>

// for NI_MAXHOST
#ifdef _WIN32
#include <Ws2tcpip.h>
#else
#include <netdb.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <array>
#include <future>
#include <thread>

namespace
{
// Transform camelCase to hyphenated-notation, e.g.
// lexis/render/LookOut -> lexis/render/look-out
// Inspiration:
// https://gist.github.com/rodamber/2558e25d4d8f6b9f2ffdf7bd49471340
std::string _camelCaseToHyphenated(const std::string& camelCase)
{
    if (camelCase.empty())
        return camelCase;

    std::string str(1, tolower(camelCase[0]));
    for (auto it = camelCase.begin() + 1; it != camelCase.end(); ++it)
    {
        if (isupper(*it) && *(it - 1) != '-' && islower(*(it - 1)))
            str += "-";
        str += *it;
    }

    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// http://stackoverflow.com/questions/5343190
std::string _replaceAll(std::string subject, const std::string& search,
                        const std::string& replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos)
    {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}

// convert name to lowercase with '/' separators instead of '::'
std::string _convertEndpointName(const std::string& endpoint)
{
    return _camelCaseToHyphenated(_replaceAll(endpoint, "::", "/"));
}

const std::string JSON_TYPE = "application/json";
const std::string REQUEST_REGISTRY = "registry";
const std::string REQUEST_SCHEMA = "schema";
const std::string HTTP_SERVER_SERVICE = "_http._tcp"; // Standard HTTP service

void _checkEndpointName(const std::string& endpoint)
{
    if (endpoint == REQUEST_REGISTRY)
        ZEROEQTHROW(
            std::runtime_error("'registry' not allowed as endpoint name"));
}

bool _endsWithSchema(const std::string& uri)
{
    if (uri.length() < REQUEST_SCHEMA.length())
        return false;
    return uri.compare(uri.length() - REQUEST_SCHEMA.length(),
                       std::string::npos, REQUEST_SCHEMA) == 0;
}

std::string _removeEndpointFromPath(const std::string& endpoint,
                                    const std::string& path)
{
    if (endpoint == "/")
        return path;

    if (endpoint.size() >= path.size())
        return std::string();

    return path.substr(endpoint.size());
}

std::string _getHost(const zeroeq::URI& uri)
{
    // INADDR_ANY translation: zmq -> boost.asio
    if (uri.getHost().empty() || uri.getHost() == "*")
        return "0.0.0.0";
    return uri.getHost();
}

bool _isCorsRequest(const zeroeq::http::Message& message)
{
    return !message.origin.empty();
}

bool _isCorsPreflightRequest(const zeroeq::http::Message& message)
{
    return _isCorsRequest(message) &&
           message.request.method == zeroeq::http::Method::OPTIONS &&
           message.accessControlRequestMethod != zeroeq::http::Method::ALL;
}
} // anonymous namespace

namespace zeroeq
{
namespace http
{
class Server::Impl : public detail::Sender
{
public:
    Impl(const URI& uri_, const std::string& session)
        : detail::Sender(URI(_getInprocURI()), ZMQ_PAIR, HTTP_SERVER_SERVICE,
                         session == DEFAULT_SESSION ? getDefaultPubSession()
                                                    : session)
        , _requestHandler(_getInprocURI())
        , _httpOptions(_requestHandler)
        , _httpServer(_httpOptions.address(_getHost(uri_))
                          .port(std::to_string(int(uri_.getPort())))
                          .protocol_family(HTTPServer::options::ipv4)
                          .reuse_address(true))
    {
        if (::zmq_bind(socket.get(), _getInprocURI().c_str()) == -1)
        {
            ZEROEQTHROW(
                std::runtime_error("Cannot bind HTTPServer to inproc socket"));
        }

        try
        {
            _httpServer.listen();
            _httpThread.reset(new std::thread([&] {
                try
                {
                    _httpServer.run();
                }
                catch (const std::exception& e)
                {
                    ZEROEQERROR
                        << "Error during HTTPServer::run(): " << e.what()
                        << std::endl;
                }
            }));
        }
        catch (const std::exception& e)
        {
            ZEROEQTHROW(std::runtime_error(
                std::string("Error while starting HTTP server: ") + e.what()));
        }

        uri = URI();
        uri.setHost(_httpServer.address());
        uri.setPort(std::stoi(_httpServer.port()));

        if (uri.getHost() == "0.0.0.0")
        {
            char hostname[NI_MAXHOST + 1] = {0};
            gethostname(hostname, NI_MAXHOST);
            hostname[NI_MAXHOST] = '\0';
            uri.setHost(hostname);
        }

        if (uri_.getHost() != uri.getHost() || uri_.getPort() == 0)
            ZEROEQINFO << "HTTP server bound to " << uri.getHost() << ":"
                       << uri.getPort() << std::endl;

        if (!servus::Servus::isAvailable())
            return;

        announce();
    }

    ~Impl()
    {
        if (_httpThread)
        {
            _httpServer.stop();
            _httpThread->join();
        }
    }

    void registerSchema(const std::string& endpoint, const std::string& schema)
    {
        const std::string exist = getSchema(endpoint);
        if (exist.empty())
            _schemas[endpoint] = schema;
        else if (schema != exist)
            ZEROEQTHROW(std::runtime_error(
                "Schema registered for endpoint differs: " + endpoint));
    }

    std::string getSchema(const std::string& endpoint) const
    {
        const auto& i = _schemas.find(endpoint);
        return i != _schemas.end() ? i->second : std::string();
    }

    bool remove(const servus::Serializable& serializable)
    {
        const auto endpoint = _convertEndpointName(serializable.getTypeName());
        _schemas.erase(endpoint);
        const bool foundPUT = _methods[int(Method::PUT)].erase(endpoint) != 0;
        const bool foundGET = _methods[int(Method::GET)].erase(endpoint) != 0;
        return foundPUT || foundGET;
    }

    bool remove(const std::string& endpoint)
    {
        _schemas.erase(endpoint);
        bool foundMethod = false;
        for (auto& method : _methods)
            if (method.erase(endpoint) != 0)
                foundMethod = true;
        return foundMethod;
    }

    bool handle(const Method method, const std::string& endpoint, RESTFunc func)
    {
        _checkEndpointName(endpoint);

        if (_methods[int(method)].count(endpoint) != 0)
            return false;

        _methods[int(method)][endpoint] = func;
        return true;
    }

    bool handlePUT(const std::string& endpoint,
                   servus::Serializable& serializable)
    {
        const auto func = [&serializable](const std::string& json) {
            return serializable.fromJSON(json);
        };
        return handlePUT(endpoint, serializable.getSchema(), func);
    }

    bool handlePUT(const std::string& endpoint, const std::string& schema,
                   const PUTPayloadFunc& func)
    {
        _checkEndpointName(endpoint);

        const auto futureFunc = [func](const Request& request) {
            const auto code = func(request.body) ? Code::OK : Code::BAD_REQUEST;
            return make_ready_response(code);
        };
        if (!handle(Method::PUT, endpoint, futureFunc))
            return false;

        if (!schema.empty())
            registerSchema(endpoint, schema);
        return true;
    }

    bool handleGET(const std::string& endpoint,
                   const servus::Serializable& serializable)
    {
        const auto func = [&serializable] { return serializable.toJSON(); };
        return handleGET(endpoint, serializable.getSchema(), func);
    }

    bool handleGET(const std::string& endpoint, const std::string& schema,
                   const GETFunc& func)
    {
        _checkEndpointName(endpoint);

        const auto futureFunc = [func](const Request&) {
            return make_ready_response(Code::OK, func(), JSON_TYPE);
        };
        if (!handle(Method::GET, endpoint, futureFunc))
            return false;

        if (!schema.empty())
            registerSchema(endpoint, schema);

        return true;
    }

    void addSockets(std::vector<detail::Socket>& entries)
    {
        detail::Socket entry;
        entry.socket = socket.get();
        entry.events = ZMQ_POLLIN;
        entries.push_back(entry);
    }

    std::string getRegistry() const
    {
        Json::Value body(Json::objectValue);
        for (const auto& i : _methods[int(Method::GET)])
            body[i.first].append("GET");
        for (const auto& i : _methods[int(Method::POST)])
            body[i.first].append("POST");
        for (const auto& i : _methods[int(Method::PUT)])
            body[i.first].append("PUT");
        for (const auto& i : _methods[int(Method::PATCH)])
            body[i.first].append("PATCH");
        for (const auto& i : _methods[int(Method::DELETE)])
            body[i.first].append("DELETE");
        for (const auto& i : _methods[int(Method::OPTIONS)])
            body[i.first].append("OPTIONS");
        return body.toStyledString();
    }

    std::string getAllowedMethods(const std::string& endpoint) const
    {
        std::string methods;
        if (_methods[int(Method::GET)].count(endpoint))
            methods.append(methods.empty() ? "GET" : ", GET");
        if (_methods[int(Method::POST)].count(endpoint))
            methods.append(methods.empty() ? "POST" : ", POST");
        if (_methods[int(Method::PUT)].count(endpoint))
            methods.append(methods.empty() ? "PUT" : ", PUT");
        if (_methods[int(Method::PATCH)].count(endpoint))
            methods.append(methods.empty() ? "PATCH" : ", PATCH");
        if (_methods[int(Method::DELETE)].count(endpoint))
            methods.append(methods.empty() ? "DELETE" : ", DELETE");
        if (_methods[int(Method::OPTIONS)].count(endpoint))
            methods.append(methods.empty() ? "OPTIONS" : ", OPTIONS");
        return methods;
    }

    std::future<Response> respondTo(Request& request) const
    {
        const auto method = request.method;
        const auto path = request.path.substr(1); // remove leading '/'

        if (method == Method::GET)
        {
            if (path == REQUEST_REGISTRY)
                return make_ready_response(Code::OK, getRegistry(), JSON_TYPE);

            if (_endsWithSchema(path))
            {
                const auto endpoint = path.substr(0, path.find_last_of('/'));
                const auto it = _schemas.find(endpoint);
                if (it != _schemas.end())
                    return make_ready_response(Code::OK, it->second, JSON_TYPE);
            }
        }

        const auto beginsWithPath = [&path](const FuncMap::value_type& pair) {
            const auto& endpoint = pair.first;
            return path.find(endpoint) == 0;
        };
        const auto& funcMap = _methods[int(method)];
        const auto it =
            std::find_if(funcMap.begin(), funcMap.end(), beginsWithPath);
        if (it != funcMap.end())
        {
            const auto& endpoint = it->first;
            const auto& func = it->second;

            const auto pathStripped = _removeEndpointFromPath(endpoint, path);
            if (pathStripped.empty() || *endpoint.rbegin() == '/')
            {
                request.path = pathStripped;
                return func(request);
            }
        }

        // if "/" is registered as an endpoint it should be passed all
        // unhandled requests.
        if (funcMap.count("/"))
        {
            const auto& func = funcMap.at("/");
            request.path = path;
            return func(request);
        }

        // return informative error 405 "Method Not Allowed" if possible
        const auto allowedMethods = getAllowedMethods(path);
        if (!allowedMethods.empty())
        {
            using Headers = std::map<Header, std::string>;
            Headers headers{{Header::ALLOW, allowedMethods}};
            return make_ready_response(Code::NOT_SUPPORTED, std::string(),
                                       std::move(headers));
        }

        return make_ready_response(Code::NOT_FOUND);
    }

    void processCorsPreflightRequest(Message& message) const
    {
        // In a typical situation, user agents discover via a preflight request
        // whether a cross-origin resource is prepared to accept requests.
        // The current implementation accepts all sources for all requests.
        // More information can be found here: https://www.w3.org/TR/cors

        // remove leading '/'
        const auto path = message.request.path.substr(1);

        const bool isSchemaRequest = _isSchemaRequest(message);
        if (!_methods[int(message.accessControlRequestMethod)].count(path) &&
            !isSchemaRequest)
        {
            message.response = make_ready_response(Code::NOT_SUPPORTED);
            return;
        }

        message.corsResponseHeaders = {
            {CorsResponseHeader::access_control_allow_headers, "Content-Type"},
            {CorsResponseHeader::access_control_allow_methods,
             isSchemaRequest ? "GET" : getAllowedMethods(path)},
            {CorsResponseHeader::access_control_allow_origin, "*"}};
        message.response = make_ready_response(Code::OK);
    }

private:
    bool _isSchemaRequest(Message& message) const
    {
        const auto path = message.request.path.substr(1);
        if (!_endsWithSchema(path))
            return false;

        const auto endpoint = path.substr(0, path.find_last_of('/'));
        if (_schemas.find(endpoint) == _schemas.end())
            return false;

        return message.accessControlRequestMethod == Method::GET;
    }

    // key stores endpoints of Serializable objects lower-case, hyphenated,
    // with '/' separators
    // must be an ordered map in order to iterate from the most specific path
    typedef std::map<std::string, RESTFunc, std::greater<std::string>> FuncMap;
    typedef std::map<std::string, std::string> SchemaMap;

    SchemaMap _schemas;
    std::array<FuncMap, size_t(Method::ALL)> _methods;
    RequestHandler _requestHandler;
    HTTPServer::options _httpOptions;
    HTTPServer _httpServer;
    std::unique_ptr<std::thread> _httpThread;

    std::string _getInprocURI() const
    {
        std::ostringstream inprocURI;
// No socket notifier possible on inproc ZMQ sockets,
// (https://github.com/zeromq/libzmq/issues/1434).
// Use inproc on Windows as ipc is not supported there, which means
// we do not support notifications on Windows...
#ifdef _MSC_VER
        inprocURI << "inproc://#" << static_cast<const void*>(this);
#else
        inprocURI << "ipc:///tmp/" << static_cast<const void*>(this);
#endif
        return inprocURI.str();
    }
};

namespace
{
std::string _getServerParameter(const int argc, const char* const* argv)
{
    for (int i = 0; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--zeroeq-http-server")
        {
            if (i == argc - 1 || argv[i + 1][0] == '-')
                return "tcp://";
            return argv[i + 1];
        }
    }
    return std::string();
}
}

Server::Server(const URI& uri, Receiver& shared)
    : Receiver(shared)
    , _impl(new Impl(uri, DEFAULT_SESSION))
{
}

Server::Server(const URI& uri)
    : Receiver()
    , _impl(new Impl(uri, DEFAULT_SESSION))
{
}

Server::Server(Receiver& shared)
    : Receiver(shared)
    , _impl(new Impl(URI(), DEFAULT_SESSION))
{
}

Server::Server()
    : Receiver()
    , _impl(new Impl(URI(), DEFAULT_SESSION))
{
}

Server::~Server()
{
}

std::unique_ptr<Server> Server::parse(const int argc, const char* const* argv)
{
    const std::string& param = _getServerParameter(argc, argv);
    if (param.empty())
        return nullptr;

    return std::unique_ptr<Server>(new Server(URI(param)));
}

std::unique_ptr<Server> Server::parse(const int argc, const char* const* argv,
                                      Receiver& shared)
{
    const std::string& param = _getServerParameter(argc, argv);
    if (param.empty())
        return nullptr;

    return std::unique_ptr<Server>(new Server(URI(param), shared));
}

const URI& Server::getURI() const
{
    return _impl->uri;
}

SocketDescriptor Server::getSocketDescriptor() const
{
#ifdef _MSC_VER
    ZEROEQTHROW(std::runtime_error(
        std::string("HTTP server socket descriptor not available")));
#else
    SocketDescriptor fd = 0;
    size_t fdLength = sizeof(fd);
    if (::zmq_getsockopt(_impl->socket.get(), ZMQ_FD, &fd, &fdLength) == -1)
    {
        ZEROEQTHROW(
            std::runtime_error(std::string("Could not get socket descriptor")));
    }
    return fd;
#endif
}

bool Server::handle(const std::string& endpoint, servus::Serializable& object)
{
    return handlePUT(endpoint, object) && handleGET(endpoint, object);
}

bool Server::handle(const Method action, const std::string& endpoint,
                    RESTFunc func)
{
    return _impl->handle(action, endpoint, func);
}

bool Server::remove(const servus::Serializable& object)
{
    return _impl->remove(object);
}

bool Server::remove(const std::string& endpoint)
{
    return _impl->remove(endpoint);
}

bool Server::handlePUT(servus::Serializable& object)
{
    return _impl->handlePUT(_convertEndpointName(object.getTypeName()), object);
}

bool Server::handlePUT(const std::string& endpoint,
                       servus::Serializable& object)
{
    return _impl->handlePUT(endpoint, object);
}

bool Server::handlePUT(const std::string& endpoint, const PUTFunc& func)
{
    return _impl->handlePUT(endpoint, "",
                            [func](const std::string&) { return func(); });
}

bool Server::handlePUT(const std::string& endpoint, const std::string& schema,
                       const PUTFunc& func)
{
    return _impl->handlePUT(endpoint, schema,
                            [func](const std::string&) { return func(); });
}

bool Server::handlePUT(const std::string& endpoint, const PUTPayloadFunc& func)
{
    return _impl->handlePUT(endpoint, "", func);
}

bool Server::handlePUT(const std::string& endpoint, const std::string& schema,
                       const PUTPayloadFunc& func)
{
    return _impl->handlePUT(endpoint, schema, func);
}

bool Server::handleGET(const servus::Serializable& object)
{
    return _impl->handleGET(_convertEndpointName(object.getTypeName()), object);
}

bool Server::handleGET(const std::string& endpoint,
                       const servus::Serializable& object)
{
    return _impl->handleGET(endpoint, object);
}

bool Server::handleGET(const std::string& endpoint, const GETFunc& func)
{
    return _impl->handleGET(endpoint, "", func);
}

bool Server::handleGET(const std::string& endpoint, const std::string& schema,
                       const GETFunc& func)
{
    return _impl->handleGET(endpoint, schema, func);
}

std::string Server::getSchema(const servus::Serializable& object) const
{
    const auto endpoint = _convertEndpointName(object.getTypeName());
    return _impl->getSchema(endpoint);
}

std::string Server::getSchema(const std::string& endpoint) const
{
    return _impl->getSchema(endpoint);
}

std::future<Response> Server::respondTo(Request& request) const
{
    return _impl->respondTo(request);
}

void Server::addSockets(std::vector<detail::Socket>& entries)
{
    _impl->addSockets(entries);
}

bool Server::process(detail::Socket&)
{
    Message* message = nullptr;
    ::zmq_recv(_impl->socket.get(), &message, sizeof(message), 0);
    if (!message)
        ZEROEQTHROW(std::runtime_error(
            "Could not receive HTTP request from HTTP server"));

    if (_isCorsPreflightRequest(*message))
    {
        _impl->processCorsPreflightRequest(*message);
    }
    else
    {
        try
        {
            message->response = respondTo(message->request);
        }
        catch (const std::exception& e)
        {
            message->response =
                make_ready_response(Code::INTERNAL_SERVER_ERROR,
                                    std::string("Request handler exception: ") +
                                        e.what());
        }
        catch (...)
        {
            message->response =
                make_ready_response(Code::INTERNAL_SERVER_ERROR,
                                    "An unknown exception occurred");
        }

        // When a client makes a CORS request (by setting an 'Origin' header) it
        // expects an 'Access-Control-Allow-Origin' response header. See:
        // https://developer.mozilla.org/en-US/docs/Web/HTTP/Access_control_CORS
        // https://www.html5rocks.com/en/tutorials/cors/
        if (_isCorsRequest(*message))
        {
            message->corsResponseHeaders = {
                {CorsResponseHeader::access_control_allow_origin, "*"}};
        }
    }

    bool done = true;
    ::zmq_send(_impl->socket.get(), &done, sizeof(done), 0);
    return true;
}
}
}
