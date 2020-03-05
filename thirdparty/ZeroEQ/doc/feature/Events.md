Event Communication {#eventbased}
============

With the introduction of servus::Serializable the default communication
pattern has changed to be object-based state modifications. There are
however use cases where an event-based communication is desirable, for
example to notify of an action to be taken.

## Requirements

* Function callbacks for events
* Support events with and without payload
* Cheap construction of ZeroBuf "around" received payload

## API

    namespace zeroeq
    {
    typedef std::function< void( const void*, size_t ) > EventFunc;
    typedef std::function< int( const void*, size_t ) > HTTPEventFunc;

    class Publisher
    {
        // Can use existing publish( const servus::Serializable& )
        // or object-free alternative?
        bool publish( const uint128_t& type,
                      const void* data, const size_t size );
    };

    class Subscriber
    {
         bool subscribe( const uint128_t& event, const EventFunc& func );
         // existing subscribe registers a lambda handler which executes
         // copyBuffer(); notifyReceived();
         // unsubscribe does not change
    };

    namespace http
    {
    class Server
    {
        // as Subscriber::subscribe(), except HTTPEventFunc returns
        // status code (200, 404, etc)
    };
    }

## Examples

    int onOpen( const void* data, const size_t size )
    {
        zerobuf::data::ConstOpenEventPtr open =
            zerobuf::data::OpenEvent::create( data, size );
        openURL( open->getURL( ));
        return 200;
    }

    void onExit( const void*, size_t )
    {
        ::exit( EXIT_SUCCESS );
    }

    void setupZeroEQ()
    {
        _httpServer.subscribe( zerobuf::data::OpenEvent::ZEROBUF_TYPE(),
                               onOpen );
        _subscriber.subscribe( zerobuf::data::Exit::ZEROBUF_TYPE(), onExit );
    }

## Implementation

* Replace object registry with function registry
* Use EventFunc lmabdas for object-based registration
* Add in zerobufCxx.py:
  * typedef std::unique_ptr< const T > ConstTPtr;
  * static create method returning ConstTPtr
  * New ConstAllocator using const memory given to create method

## Issues
