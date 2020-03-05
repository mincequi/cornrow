HTTP Server for ZeroEq Events {#httpserver}
============

The http::Server implements a zeroeq::Receiver and Sender to serve ZeroBuf
objects through a REST interface with JSON payload. It is the evolution
of the RESTBridge. [Issue 115](https://github.com/HBPVIS/ZeroEQ/issues/115)
tracks the implementation.


## Requirements

* Translate Zerobufs to and from JSON
* Accept PUT requests to update subscribed Zerobuf in the same
  way as a zeroeq::Subscriber
* Accept GET requests and reply with the current state of registered
  Zerobuf in the same way as a Publisher::publish().
* Allow custom code execution while handling a GET request, e.g., to
  render a new frame before serving a ImageJPEG request
* Implement the same client REST API as the current RESTBridge
* Do not accept POST requests (as RestBridge does for updates): They are
  for adding objects, which we might introduce later.

## Dependency Changes

* Remove: RESTBridge, cppnetlib
+ Add: httpxx http parsing library, will be subproject, replaces cppnetlib

## API

    namespace servus
    {
    /** Interface for serializable objects */
    class Serializable
    {
    public:
        struct Data
        {
            std::shared_ptr< void* > ptr;
            size_t size;
        };

        virtual uint128_t getTypeIdentifier() const = 0;

        virtual void fromBinary( const void* data, const size_t size );
        virtual Data toBinary() const;

        virtual void fromJSON( const std::string& json );
        virtual std::string toJSON() const;

        /**
         * Register a function called after the object has been updated remotely
         * (via a subscriber, a http server, loading from file...).
         * Only one callback is supported at the moment, to deregister the callback,
         * call this function with a 'nullptr' (or 0) parameter.
         *
         * @throw if a DeserializedCallback is already registered and the specified
         * callback is not 'nullptr' (or 0)
         */
        SERVUS_API void registerDeserializedCallback( const DeserializedCallback& );

        /**
         * Register a function to be called when the serializable object is about
         * to be serialized.
         * Only one callback is supported at the moment, to deregister the callback,
         * call this function with a 'nullptr' (or 0) parameter.
         *
         * @throw if a SerializedCallback is already registered and the specified
         * callback is not 'nullptr' (or 0)
         */
        SERVUS_API void registerSerializeCallback( const SerializeCallback& );
    };
    }

    namespace zeroeq
    {
    namespace http
    {
    class Server : public zeroeq::Receiver
    {
    public:
        // throws if scheme is not empty or tcp
        explicit Server( const URI& uri );
        Server( const URI& uri, Receiver& shared );

        /**
         * Create a new Server when requested.
         *
         * The creation and parameters depend on the following command line
         * parameters:
         * * --zeroeq-http-server [host][:port]: Enable the server. Optional
         *   parameters configure the web server, running by default on :4020
         */
        static std::unique_ptr< Server > parse( argc, argv );
        static std::unique_ptr< Server > parse( argc, argv, Receiver& shared );

        bool handle( servus::Serializable& object );
        bool remove( const servus::Serializable& object );

        bool handlePUT( servus::Serializable& object );
        bool removePUT( const servus::Serializable& object );

        bool handleGET( servus::Serializable& object );
        bool removeGET( const servus::Serializable& object );

        // + 8 overloads with PUTFunc, PUTPayloadFunc, GETFunc
    };
    }
    }

## Examples

    void livre::Communicator::_setupRESTBridge( const int argc, char** argv )
    {
        _httpServer = zeroeq::http::Server::parse( argc, argv );
        if( !_httpServer )
            return;

        subscribers.push_back( _httpServer );
        _httpServer.handle( camera, LUT, vrParams );
        _httpServer.handleGET( frame );
    }

    class livre::ImageJPEG : public zeroeq::hbp::ImageJPEG
    {
    protected:
        void notifyRequested() final
        {
            config->frame(); // redraw before serving the last image
        }
   };

## Implementation

* Hooked into ZeroEQ as a stream zmq socket:
  http://api.zeromq.org/4-1:zmq-socket#toc22
* Implemented as a shared receiver, that is, can be grouped with other
  subscribers and servers for a single receive()
* Processes data using a HTTP parsing library (see Issue 2)
* Remove generic Schema-based JSON conversion
  * Generate to/fromJSON in code generator
  * Remove Schema and JsonConverter

### Server pseudo-code
    void http::Server::process( zeroeq::detail::Socket& socket )
    {
        zmq_recv, feed to message
        if message is complete
             for PUT:
                 _subscribed[ type ].fromJSON( message.body( ))
                 _subscribed[ type ].notifyUpdated()
                 zmq_send( 200/404 )
             for GET:
                 _registered[ type ].notifyRequested()
                 zmq_send( 200 + _registered[ type ].toJSON( ))

    }

## Issues

### 1: Shall we remove the generic JSON conversion?

Resolved: Yes, when maintenance becomes a burden.

Removing the generic conversion and emitting specific code in the
generator simplifies the implementation significantly. It does however
remove the capability to translate to and from JSON in an indepent
component which does not have access to the application-specific
vocabulary. Today this is only used when the REST bridge is run as a
separate process.

To not impose Zerobuf on other applications (e.g. for the visualization
web services), the servus::Serializable interface defines the minimal API used
by the zeroeq::Subscriber and zeroeq::http::Server, to be implemented by
applications (and ZeroBuf).

### 2: How do we parse HTTP?

Resolved: Use httpxx. Fix CMake as needed to integrate as a sub project.

Candidates:
* Write code: reject, NIH.
* libghttp: Hard to find repository, C code, seems to live in Gnome TCP
  socket now
* https://github.com/AndreLouisCaron/httpxx
  * cmake, builds on Ubuntu, no install rules, not sub-project ready, small
  * Example: https://github.com/AndreLouisCaron/httpxx/blob/master/demo/parse-in-random-increments.cpp
* All other candidates where a full http server
  * Mongrel2: A full-blown, programmable, scalable web server with its
    own network code (ie. not ZeroMQ-glueable)
  * zurl: A simple web server using ZeroMQ and Qt, large dependencies for
    the feature set

### 3: What about SSL/TLS?

Resolved: Delay, later either use proxy http servers or do a native
integration.

The native ZeroEQ integration using a zmq socket enables seamless
application integration. For an application, a http::Server behaves as a
Subscriber, and they can be integrated in a shared receiver
group. Unfortunately there is little prior work on SSL/TLS over zmq
stream sockets. The easiest seems to be using a proxy frontend server to
shield the (anyways unprotected) applications. The more involved seems
to be integrate OpenSSL (or similar) with zmq:

* http://www.riskcompletefailure.com/2012/09/tls-and-zeromq.html
* https://github.com/ianbarber/TLSZMQ
