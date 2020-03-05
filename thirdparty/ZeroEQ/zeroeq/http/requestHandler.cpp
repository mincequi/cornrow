
/* Copyright (c) 2016-2017, Human Brain Project
 *                          Stefan.Eilemann@epfl.ch
 *                          Daniel.Nachbaur@epfl.ch
 *                          Raphael.Dumusc@epfl.ch
 */

#include "requestHandler.h"

#include <zeroeq/log.h>
#include <zeroeq/uri.h>

#include <zmq.h>

#include <future>
#include <memory> // shared_from_this
#include <stdexcept>

namespace zeroeq
{
namespace http
{
namespace
{
size_t _getContentLength(const HTTPServer::request& request)
{
    for (const auto& i : request.headers)
    {
        if (i.name == "Content-Length")
        {
            int size = std::stoi(i.value);
            return (size > 0) ? size : 0;
        }
    }
    return 0;
}

Method _getMethodType(const std::string& methodName)
{
    if (methodName == "GET")
        return Method::GET;
    if (methodName == "POST")
        return Method::POST;
    if (methodName == "PUT")
        return Method::PUT;
    if (methodName == "PATCH")
        return Method::PATCH;
    if (methodName == "DELETE")
        return Method::DELETE;
    if (methodName == "OPTIONS")
        return Method::OPTIONS;
    throw std::invalid_argument("Method not supported");
}

std::string _headerEnumToString(const Header header)
{
    switch (header)
    {
    case Header::ALLOW:
        return "Allow";
    case Header::CONTENT_TYPE:
        return "Content-Type";
    case Header::LAST_MODIFIED:
        return "Last-Modified";
    case Header::LOCATION:
        return "Location";
    case Header::RETRY_AFTER:
        return "Retry-After";
    default:
        throw std::logic_error("no such header");
    }
}

std::string _headerEnumToString(const CorsResponseHeader header)
{
    switch (header)
    {
    case CorsResponseHeader::access_control_allow_headers:
        return "Access-Control-Allow-Headers";
    case CorsResponseHeader::access_control_allow_methods:
        return "Access-Control-Allow-Methods";
    case CorsResponseHeader::access_control_allow_origin:
        return "Access-Control-Allow-Origin";
    default:
        throw std::logic_error("no such header");
    }
}

// The actual handler for each incoming request where the data is read from
// a dedicated connection to the client.
struct ConnectionHandler : std::enable_shared_from_this<ConnectionHandler>
{
    ConnectionHandler(const HTTPServer::request& request, void* socket)
        : _request(request)
        , _socket(socket)
    {
    }

    void operator()(HTTPServer::connection_ptr connection)
    {
        try
        {
            const auto method = _getMethodType(_request.method);
            if (method != Method::GET)
            {
                _size = _getContentLength(_request);
                // if we have payload, schedule an (async) read of all chunks.
                // Will call _handleRequest() after all data has been read.
                if (_size > 0)
                {
                    _readChunk(connection, method);
                    return;
                }
            }
            _handleRequest(method, connection);
        }
        catch (const std::invalid_argument&)
        {
            connection->set_status(HTTPServer::connection::not_supported);
        }
    }

private:
    void _readChunk(HTTPServer::connection_ptr connection, const Method method)
    {
        namespace pl = std::placeholders;
        connection->read(std::bind(&ConnectionHandler::_handleChunk,
                                   ConnectionHandler::shared_from_this(),
                                   pl::_1, pl::_2, pl::_3, connection, method));
    }

    void _handleChunk(HTTPServer::connection::input_range range,
                      const boost::system::error_code error, const size_t size,
                      HTTPServer::connection_ptr connection,
                      const Method method_)
    {
        if (error)
        {
            ZEROEQERROR << "Error during ConnectionHandler::_handleChunk: "
                        << error.message() << std::endl;
            return;
        }

        _body.append(&range[0], size);
        _size -= size;
        if (_size > 0)
            _readChunk(connection, method_);
        else
            _handleRequest(method_, connection);
    }

    void _handleRequest(const Method method,
                        HTTPServer::connection_ptr connection)
    {
        Message message;
        message.request.method = method;
        message.request.source = _request.source;
        const auto uri = URI(_request.destination);
        message.request.path = uri.getPath();
        message.request.query = uri.getQuery();
        message.request.body.swap(_body);
        _parseCorsRequestHeaders(message);

        void* messagePtr = &message;
        zmq_send(_socket, &messagePtr, sizeof(void*), 0);
        bool done;
        zmq_recv(_socket, &done, sizeof(done), 0);

        Response response;
        try
        {
            response = message.response.get();
        }
        catch (std::future_error& error)
        {
            response.code = http::Code::INTERNAL_SERVER_ERROR;
            ZEROEQINFO << "Error during ConnectionHandler::_handleRequest: "
                       << error.what() << std::endl;
        }

        std::vector<HTTPServer::response_header> headers;
        headers.push_back(
            {"Content-Length", std::to_string(response.body.length())});

        for (const auto& it : message.corsResponseHeaders)
            headers.push_back({_headerEnumToString(it.first), it.second});

        for (const auto& it : response.headers)
            headers.push_back({_headerEnumToString(it.first), it.second});

        const auto status = HTTPServer::connection::status_t(response.code);
        connection->set_status(status);
        connection->set_headers(headers);
        connection->write(response.body);
    }

    void _parseCorsRequestHeaders(Message& message)
    {
        for (const auto& header : _request.headers)
        {
            if (header.name == "Origin")
                message.origin = header.value;
            else if (header.name == "Access-Control-Request-Headers")
                message.accessControlRequestHeaders = header.value;
            else if (header.name == "Access-Control-Request-Method")
            {
                try
                {
                    message.accessControlRequestMethod =
                        _getMethodType(header.value);
                }
                catch (const std::invalid_argument&)
                {
                }
            }
        }
    }

    const HTTPServer::request& _request;
    void* _socket;
    std::string _body;
    size_t _size = 0;
};
} // anonymous namespace

RequestHandler::RequestHandler(const std::string& zmqURL)
    : _context(detail::getContext())
    , _socket(zmq_socket(_context.get(), ZMQ_PAIR))
{
    if (zmq_connect(_socket, zmqURL.c_str()) == -1)
    {
        ZEROEQTHROW(std::runtime_error(
            "Cannot connect RequestHandler to inproc socket"));
    }
}

RequestHandler::~RequestHandler()
{
    zmq_close(_socket);
}

void RequestHandler::operator()(const HTTPServer::request& request,
                                HTTPServer::connection_ptr connection)
{
    // as the underlying cppnetlib http server is asynchronous and payload for
    // PUT events has to be read in chunks in the cppnetlib thread, create
    // a shared instance of the handler object that is passed to cppnetlib for
    // processing the request.
    std::shared_ptr<ConnectionHandler> connectionHandler(
        new ConnectionHandler(request, _socket));
    (*connectionHandler)(connection);
}
}
}
