# Changelog {#Changelog}

# Release 0.9 (06-02-2018)

* [226](https://github.com/HBPVIS/ZeroEQ/pull/226):
  Better failsafe when announcement is not supported by Servus
* [225](https://github.com/HBPVIS/ZeroEQ/pull/225):
  Fix #224: Handle exceptions in server handlers
* [219](https://github.com/HBPVIS/ZeroEQ/pull/219):
  Implement Client-Server req-rep support
  * The environment variables ZEROEQ_PUB_SESSION and ZEROEQ_SERVER_SESSION
    replace the now deprecated ZEROEQ_SESSION variable.
* [223](https://github.com/HBPVIS/ZeroEQ/pull/223):
  Subscriber(uri) ctors are replaced by Subscriber(uris)
* [218](https://github.com/HBPVIS/ZeroEQ/pull/218):
  Fix infinite loop in slow receivers
* [215](https://github.com/HBPVIS/ZeroEQ/pull/215):
  Implement Monitor to receive notifications for new subscribers
* [217](https://github.com/HBPVIS/ZeroEQ/pull/217):
  Fix HTTP server response to CORS requests ("Access-Control-Allow-Origin: *"
  was missing).
* [216](https://github.com/HBPVIS/ZeroEQ/pull/216):
  http::Server can be subclassed to filter requests, which can be useful to
  restrict access to selected hosts or methods.
* [213](https://github.com/HBPVIS/ZeroEQ/pull/213):
  Remove ambiguous Subscriber(uri, session) ctors

# Release 0.8 (24-05-2017)

* [210](https://github.com/HBPVIS/ZeroEQ/pull/210):
  http::Server:
    * Fix handling of CORS preflight requests
    * Expose OPTIONS http method
* [208](https://github.com/HBPVIS/ZeroEQ/pull/208):
  Zeroconf announce for http::Server
* [207](https://github.com/HBPVIS/ZeroEQ/pull/207):
  Unify name resolution between Publisher and http::Server
* [203](https://github.com/HBPVIS/ZeroEQ/pull/203):
  HTTP server extension for generic REST interfaces.
  Users can now:
    * use all standard HTTP verbs: GET, POST, PUT, PATCH, DELETE
    * set a response header, including content type
    * set a custom response payload for all action verbs
    * use a std::future to avoid blocking the main thread
    * register a single endpoint for collections
    * filter collections using a query string with GET requests
* [203](https://github.com/HBPVIS/ZeroEQ/pull/203):
  Upgrade to upstream cppnet-lib 0.13-release
* [192](https://github.com/HBPVIS/ZeroEQ/pull/192):
  Fix HTTP server segfault when registering an empty endpoint, throw an
  exception instead.
* [190](https://github.com/HBPVIS/ZeroEQ/pull/190):
  Fix HTTP server segfault when accessing the root element (GET '/').
* [189](https://github.com/HBPVIS/ZeroEQ/pull/189):
  Fix missing/wrong HTTP server socket notifications from
  zeroeq::http::Server::getSocketDescriptor(); clients are now guaranteed to be
  properly notified on read/incoming messages. This feature is not supported on
  Windows.
* [188](https://github.com/HBPVIS/ZeroEQ/pull/188):
  * Support custom HTTP URL endpoint for handled Serializables
  * Expose hyphenated URLs for CamelCase event names

# Release 0.7 (09-12-2016)

* [185](https://github.com/HBPVIS/ZeroEQ/pull/185):
  zeroeq::http::Server::getSocketDescriptor() notifies on write after
  [#184](https://github.com/HBPVIS/ZeroEQ/pull/184), not anymore on read
* [184](https://github.com/HBPVIS/ZeroEQ/pull/184):
  Use cppnetlib to implement HTTP server to support multiple clients
* [182](https://github.com/HBPVIS/ZeroEQ/pull/182):
  * Clearer HTTP server registration methods:
    * handle( serializable ) instead of add()
    * handlePUT( serializable/event ) instead of subscribe()
    * handleGET( serializable/event ) instead of register_()
    * remove( serializable/event ) instead of unsubscribe() and unregister()
  * HTTP server REST API
    * /registry endpoint returns handled events/objects
    * <registry-item>/schema endpoints return schema describing the
      events/objects. For servus::Serializable objects the schema is provided
      via getSchema(), for events the schema can be optionally provided via
      handlePUT() and handleGET()
* [179](https://github.com/HBPVIS/ZeroEQ/pull/179):
  Fix http server blocking issues with libzmq 4.1.4

# Release 0.6 (30-06-2016)

* [170](https://github.com/HBPVIS/ZeroEQ/pull/170):
  Remove FlatBuffers dependency and support
* [169](https://github.com/HBPVIS/ZeroEQ/pull/169):
  Fix #157: http server may hang in receive() when remote connection is closed
  abruptly
* [167](https://github.com/HBPVIS/ZeroEQ/pull/167):
  Move HBP vocabulary to Lexis
* [166](https://github.com/HBPVIS/ZeroEQ/pull/166):
  Implement event-based communication as per
  [152](https://github.com/HBPVIS/ZeroEQ/pull/152)
* [161](https://github.com/HBPVIS/ZeroEQ/pull/161):
  Move progressMonitor tool to Lexis
* [145](https://github.com/HBPVIS/ZeroEQ/pull/145):
  * The zeroeq::Event class is renamed to zeroeq::FBEvent
  * zeroeq::FBEvent is derived from servus::Serializable
  * Removed deprecated zeroeq::FBEvent functions from pub/sub
  * Removed EVENT_EXIT
  * Removed RESTBridge related code
  * FlatBuffers is an optional dependency
* [143](https://github.com/HBPVIS/ZeroEQ/pull/143):
  Rename zeq to ZeroEQ in the whole project
* [141](https://github.com/HBPVIS/ZeroEQ/pull/141):
  * Removed deprecated servus::URI based constructors from the pub/sub API
  * The returned URIs from pub/sub and http server is zeq::URI
  * The zeq::URI construction from servus::URI is explicit
* [140](https://github.com/HBPVIS/ZeroEQ/pull/140):
  Added getSocketDescriptor() to http::server
* [138](https://github.com/HBPVIS/ZeroEQ/pull/138):
  Command line progress monitor

# Release 0.5 (10-03-2016)

* [132](https://github.com/HBPVIS/ZeroEQ/pull/132):
  Deprecate FlatBuffers based API in publisher/subscriber
* [129](https://github.com/HBPVIS/ZeroEQ/pull/129):
  Added GET and PUT notifications to http server
* [121](https://github.com/HBPVIS/ZeroEQ/pull/121):
  zeq::URI allows to use ":port" only
* [116](https://github.com/HBPVIS/zeq/issues/115):
  Add zeq::http::Server
* [116](https://github.com/HBPVIS/ZeroEQ/pull/116):
  Generalize Zerobuf support into abstract servus::Serializable
* [109](https://github.com/HBPVIS/ZeroEQ/pull/109):
  Implement session filtering from Sessions.md RFC
  * Custom/user-defined schemas in URIs for publisher and subscriber are used
    as session name. Porting to new constructors with explicit session name
    is recommended if fixed session name is desired.
  * zeq::Publisher::getPort() removed, use
    zeq::Publisher::getURI().getPort() instead

# Release 0.4 (02-11-2015)

* [98](https://github.com/HBPVIS/ZeroEQ/pull/98):
  Disable zeroconf subscriptions to publishers in the same process
* [97](https://github.com/HBPVIS/ZeroEQ/pull/97):
  Added a small command line tool to send events parsed from a script file.
* [94](https://github.com/HBPVIS/ZeroEQ/pull/94):
  Added CellSetBinaryOp to HBP vocabulary
* [81](https://github.com/HBPVIS/ZeroEQ/pull/81):
  Implement (optional) ZeroBuf support

# Release 0.3 (07-07-2015)

* [75](https://github.com/HBPVIS/ZeroEQ/pull/75):
  Event for frame setting and animation
* [74](https://github.com/HBPVIS/ZeroEQ/pull/74):
  Fix exception with broken DNS and zeroconf
* [69](https://github.com/HBPVIS/ZeroEQ/pull/69):
  Replaced Lunchbox by Servus
* [68](https://github.com/HBPVIS/ZeroEQ/pull/68):
  OPT: Filter messages for registered handlers
* [64](https://github.com/HBPVIS/ZeroEQ/pull/64):
  Allow specification of network announce protocols in Publisher
* [63](https://github.com/HBPVIS/ZeroEQ/pull/63):
  Replace boost by C++11 std equivalents

# Release 0.2 (01-05-2015)

* zeq::connection::Broker and zeq::connection::Service for subscription from a
  remote publisher.
* Concurrently dispatch events for multiple zeq::Subscriber and
  zeq::connection::Broker using shared zeq::Receiver groups.

# Release 0.1 (07-01-2015)

* zeq::Publisher, zeq::Subscriber and zeq::Event for publish-subcribe event
  driven architectures.
