Request-Reply {#reqrep}
============

Certain algorithms need synchronous, RPC-like exchanges. This specification
describes a client-server API based on zmq req-rep (dealer/router) sockets.

## Requirements

* Asynchronous remote procedure call with later return synchronization from a Client to a Server

## API

    using ReplyFunc = std::function<void(const uint128_t&, const void*, size_t)>;

    class Client : public Receiver
    {
        // ctors same as Subscriber
        /** @return false on send error */
        bool request(const Serializable& request, ReplyFunc& func);
    };

    using ReplyData = std::pair< uint128_t, servus::Serializable::Data >;
    using HandleFunc = std::function<ReplyData(const void*, size_t)>;

    class Server : public Receiver
    {
        // ctors as Publisher
        void handle(const uint128_t& request, HandleFunc& func)
    };


## Implementation

* Maps closely to ZeroMQ
  * Client uses a dealer socket, receive() will call ReplyFunc
  * Server::receive() uses a rep socket
    * Single-threaded, synchronous for now
    * Can be extended to async model later
    * does recv()->send()
* Client::request is fully asynchronous
* ReplyFunc will get ```0, nullptr, 0``` if server has no handle func

## Examples

### Get-Set of Objects

Debatable use case: get and set will be served by one of the connected servers.
The get and set need to call receive(), potentially serving other pending requests.

    Object object;
    server.handle(zeroeq::GET | zeroeq::SET, object ;
    while(running)
        server.receive();

    ObjectPtr object = client.get<Object>();
    object->setFoo(bar);
    client.set(*object);

    template<class O> std::unique_ptr<O> Client::get()
    {
        std::unique_ptr<O> object;
        request( zeroeq::GetObject( O::ZEROBUF_TYPE_IDENTIFIER()),
            [&]( const uint128_t& type, const void* data, const size_t size ){
                check type match
                object = new O;
                object->fromBinary(data, size);
            });

        while( !object && timeout > 0 )
        {
            receive(timeout);
            update timeout;
        }
        return object;
    }

### Livre Remote DataSource

    class livre::RemoteDataSource
    {
        RemoteDataSource(const DataSourcePluginData& initData)
        {
            _client.request( initData.uri,
                [&]( const uint128_t& type, const void* data, const size_t size ){
                    handle mismatch return type
                    initData.info = *VolumeInformation::create( data, size );
                });
            _client.receive( TIMEOUT );
        }

        MemoryUnitPtr getData(const LODNode& node)
        {
            MemoryUnitPtr block;
            _client.request({node, _uri},
                        [&]( const uint128_t& type, const void* data, const size_t size ){
                            handle mismatch return type
                            block = new AllocMemoryUnit(size);
                            memcpy data to block
                        });

            _client.receive( TIMEOUT );
            return block;
        }

        zeroeq::Client _client;
    };

    class livre::RemoteDataService
    {
        RemoteDataService(const URI& uri) // does not return
        {
            _server.handle(URI::getTypeIdentifier(), &_getInfo);
            _server.handle(LODNode::getTypeIdentifier(), &_getData);

            while( _server.receive()) /*nop*/ ;
        }

        zeroeq::Server _server;

        ReplyData _getInfo(const void* data, const size_t size)
        {
            ConstURIPtr uri = livre::URI::create(data, size);
            if( openAndCacheDatasource( *uri ))
                return { info.getTypeIdentifier(), info.toBinary() };
            return {};
        }

        ReplyData _getData(const void* data, const size_t size)
        {
            ConstLODNodeURIPtr node = livre::LODNodeURI::create(data, size);
            // open data source if needed, error handling, ...
            const auto& data = dataSource.getData(*node);
            return {data.getTypeIdentifier(), data.toBinary()};
        }
    };

## Issues

### 1: Shall the handle function return a future like the http::Server?

_Resolution: No:_ The http::Server manages internal thread(s) for the protocol
handling. The zmq rep socket does not, and a multithreaded server would still
call handle() synchronously from each worker thread.

### 2: Is the client a receiver?

_Resolution: Yes:_ The get method only sends the request, the reply is received
in receive() and correlated to the request.

### 3: Do we need to type the reply data?

_Resolution: Yes:_ If the return value data is not typed, it requires that the
client and server use the same object and version for this, i.e., the hard
versioning of ZeroBuf wrt member layout is lost. The ID is not part of the
ZeroBuf, so it needs separate transport and checking in server/client. This also
allows different return types for a single request.

### 4. Should Client::request() return a future?

_Resolution: No:_ This complicates the implementation and contract. It can be
layered on top easily.

The proposed API makes the contract explicit: All requests on a client are sent
round-robin to all connected servers, and replies arrive in a random order in
receive. A future-based API would need to de-multiplex and buffer the incoming
requests to the corresponding futures, either from future.get() or .wait() or a
separate thread. A user can as well track the outstanding requests, either by
the objects sent back or through futures, and still has the full control when
this happens. When a common use case *and the associated contract* becomes clear
this can be implemented on top of this API.
