Subscription Monitor {#monitor}
============

To enable an application to republish all its state when a new subscriber
connected, we need a way to monitor connection requests. This RFC describes this
feature for this use case, based on zmq_socket_monitor.

## API

    class Sender
    {
        virtual void* getSocket() = 0;
    };

    class Publisher : public Sender {};

    class Monitor : public Receiver
    {
        Monitor( Sender& sender );
        Monitor( Sender& sender, Receiver& shared );

        virtual void notifyNewConnection() {}
        // other notifies and params lazy, on use case
    };

## Examples

   Livre Communicator derives from Monitor and republishes all events on notify.


## Implementation

### Publisher socket

Change zeroeq::Publisher socket to ZMQ_XPUB to receive topic subscriptions.
Follow the [Meerkat pattern](http://hintjens.com/blog:37) to count subscribers.

### Other sockets

Monitor installs zmq_socket_monitor() on Sender::getSocket(). Monitor::process()
calls Monitor::notifyNewConnection().


## Issues

### Issue 1: zmq_socket_monitor does not support notifications.

_Resolution: Use Meerkat pattern_. The initial implementation showed that
pub/sub sockets [do not support
notifications](https://github.com/zeromq/libzmq/issues/2618). The [Meerkat
pattern](http://hintjens.com/blog:37) explains the proper way to do this.
