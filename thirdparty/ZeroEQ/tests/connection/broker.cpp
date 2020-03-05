
/* Copyright (c) 2014-2017, Human Brain Project
 *                          Stefan.Eilemann@epfl.ch
 *                          Juan Hernando <jhernando@fi.upm.es>
 */

#define BOOST_TEST_MODULE zeroeq_connection_broker

#include "../common.h"
#include <zeroeq/connection/broker.h>

#include <servus/servus.h>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

typedef std::unique_ptr<zeroeq::connection::Broker> BrokerPtr;
std::string _broker;
zeroeq::Publisher* _publisher = 0;

class Subscriber
{
public:
    Subscriber()
        : received(false)
        , _state(STATE_CREATED)
    {
    }

    virtual ~Subscriber() {}
    void run()
    {
        zeroeq::Subscriber subscriber(test::buildURI("127.0.0.1", *_publisher));

        BOOST_CHECK(subscriber.subscribe(
            test::Echo::IDENTIFIER(),
            zeroeq::EventPayloadFunc([&](const void* data, const size_t size) {
                test::onEchoEvent(data, size);
                received = true;
            })));

        // Using the connection broker in place of zeroconf
        BrokerPtr broker = createBroker(subscriber);
        BOOST_REQUIRE(broker.get());
        if (!broker)
            return;

        _broker = broker->getAddress();
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _state = STATE_STARTED;
            _condition.notify_all();
        }

        // test receive of data for echo event
        for (size_t i = 0; i < 10; ++i)
        {
            subscriber.receive(100);
            if (received)
                return;
        }
        BOOST_CHECK(!"reachable");
    }

    void waitStarted() const
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while (_state < STATE_STARTED)
            _condition.wait(lock);
    }

    void setRun()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _state = STATE_RUN;
        _condition.notify_all();
    }

    void waitRun() const
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while (_state < STATE_RUN)
            _condition.wait(lock);
    }

    bool received;

protected:
    mutable std::condition_variable _condition;
    mutable std::mutex _mutex;
    enum State
    {
        STATE_CREATED,
        STATE_STARTED,
        STATE_RUN
    } _state;

    virtual BrokerPtr createBroker(zeroeq::Subscriber& subscriber)
    {
        return BrokerPtr(
            new zeroeq::connection::Broker("127.0.0.1:0", subscriber));
    }
};

BOOST_AUTO_TEST_CASE(broker)
{
    zeroeq::Publisher publisher(zeroeq::NULL_SESSION);
    _publisher = &publisher;
    _broker.clear();

    Subscriber subscriber;
    std::thread thread(std::bind(&Subscriber::run, &subscriber));
    subscriber.waitStarted();

    BOOST_CHECK(zeroeq::connection::Service::subscribe(_broker, publisher));
    for (size_t i = 0; i < 10 && !subscriber.received; ++i)
    {
        BOOST_CHECK(publisher.publish(test::Echo(test::echoMessage)));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    thread.join();
    BOOST_CHECK(subscriber.received);
    _publisher = 0;
}

template <zeroeq::connection::Broker::PortSelection mode>
class NamedSubscriber : public Subscriber
{
    BrokerPtr createBroker(zeroeq::Subscriber& subscriber) override
    {
        // Multiple instances of the test may run concurrently. Try until we get
        // the well-defined port
        size_t nTries = 10;
        while (nTries--)
        {
            try
            {
                return BrokerPtr(new zeroeq::connection::Broker(
                    "zeroeq::connection::test_named_broker", subscriber, mode));
            }
            catch (...)
            {
            }

            waitRun();
        }
        return BrokerPtr();
    }
};

typedef NamedSubscriber<zeroeq::connection::Broker::PORT_FIXED>
    FixedNamedSubscriber;
typedef NamedSubscriber<zeroeq::connection::Broker::PORT_FIXED_OR_RANDOM>
    RandomNamedSubscriber;

BOOST_AUTO_TEST_CASE(named_broker)
{
    zeroeq::Publisher publisher(zeroeq::NULL_SESSION);
    _publisher = &publisher;

    FixedNamedSubscriber subscriber1;
    std::thread thread1(std::bind(&Subscriber::run, &subscriber1));
    subscriber1.waitStarted();

    RandomNamedSubscriber subscriber2;
    subscriber2.received = true;
    std::thread thread2(std::bind(&Subscriber::run, &subscriber2));

    subscriber1.setRun();
    subscriber2.setRun();

    BOOST_CHECK(zeroeq::connection::Service::subscribe(
        "127.0.0.1", "zeroeq::connection::test_named_broker", publisher));

    for (size_t i = 0; i < 10 && !subscriber1.received; ++i)
    {
        BOOST_CHECK(publisher.publish(test::Echo(test::echoMessage)));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    thread2.join();
    thread1.join();
    BOOST_CHECK(subscriber1.received);
    _publisher = 0;
}

class FailingNamedSubscriber : public Subscriber
{
    BrokerPtr createBroker(zeroeq::Subscriber& subscriber) override
    {
        BOOST_CHECK_THROW(new zeroeq::connection::Broker(
                              "zeroeq::connection::test_named_broker",
                              subscriber,
                              zeroeq::connection::Broker::PORT_FIXED),
                          std::runtime_error);

        return BrokerPtr(new zeroeq::connection::Broker(
            "zeroeq::connection::test_named_broker", subscriber,
            zeroeq::connection::Broker::PORT_FIXED_OR_RANDOM));
    }
};

BOOST_AUTO_TEST_CASE(named_broker_port_used)
{
    if (getenv("TRAVIS"))
        return;

    zeroeq::Publisher publisher(zeroeq::NULL_SESSION);
    _publisher = &publisher;

    FixedNamedSubscriber subscriber1;
    std::thread thread1(std::bind(&Subscriber::run, &subscriber1));
    subscriber1.waitStarted();

    FailingNamedSubscriber subscriber2;
    subscriber2.received = true;
    std::thread thread2(std::bind(&Subscriber::run, &subscriber2));
    subscriber2.waitStarted();

    subscriber1.received = true;
    subscriber1.setRun();
    thread2.join();
    thread1.join();

    _publisher = 0;
}

BOOST_AUTO_TEST_CASE(invalid_broker)
{
    zeroeq::Subscriber subscriber(zeroeq::URI("127.0.0.1:1234"));
    BOOST_CHECK_THROW(zeroeq::connection::Broker(std::string("invalidIP"),
                                                 subscriber),
                      std::runtime_error);
}
