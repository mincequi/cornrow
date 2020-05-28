
/* Copyright (c) 2014-2017, Human Brain Project
 *                          Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *                          Stefan.Eilemann@epfl.ch
 */

#define BOOST_TEST_MODULE zeroeq_pub_sub

#include "common.h"
#include <zeroeq/detail/sender.h>

#include <servus/servus.h>
#include <servus/uri.h>

#include <chrono>
#include <thread>

BOOST_AUTO_TEST_CASE(publish_receive_serializable)
{
    test::Echo echoOut("The quick brown fox");
    test::Echo echoIn;

    zeroeq::Publisher publisher(zeroeq::NULL_SESSION);
    zeroeq::Subscriber subscriber(publisher.getURI());
    test::Monitor monitor(publisher);

    BOOST_CHECK(subscriber.subscribe(echoIn));

    for (size_t i = 0; i < 10; ++i)
    {
        BOOST_CHECK(publisher.publish(echoOut));

        if (subscriber.receive(100))
        {
            BOOST_CHECK_EQUAL(echoIn.getMessage(), echoOut.getMessage());
            BOOST_CHECK_EQUAL(monitor.connections, 0);

            for (size_t j = 0; j < 10; ++j)
            {
                BOOST_CHECK(!subscriber.receive(100));
                BOOST_CHECK_EQUAL(monitor.connections, 0);
                BOOST_CHECK(monitor.receive(500));
                BOOST_CHECK_EQUAL(monitor.connections, 1);
                return;
            }
            BOOST_CHECK(!"reachable");
            return;
        }
    }
    BOOST_CHECK(!"reachable");
}

BOOST_AUTO_TEST_CASE(publish_receive_event)
{
    const std::string echoString("The quick brown fox");
    zeroeq::Publisher publisher(
        zeroeq::URI("inproc://zeroeq.test.publish_receive_event"),
        zeroeq::NULL_SESSION);
    zeroeq::Subscriber subscriber(publisher.getURI());
    test::Monitor monitor(publisher, subscriber);

    bool received = false;
    BOOST_CHECK(subscriber.subscribe(
        zeroeq::make_uint128("Echo"),
        zeroeq::EventPayloadFunc([&](const void* data, const size_t size) {
            BOOST_CHECK_EQUAL(std::string(reinterpret_cast<const char*>(data),
                                          size),
                              echoString);
            BOOST_CHECK_EQUAL(size, echoString.length());
            received = true;
        })));

    for (size_t i = 0; i < 10; ++i)
    {
        BOOST_CHECK(publisher.publish(zeroeq::make_uint128("Echo"),
                                      echoString.c_str(), echoString.length()));

        if (subscriber.receive(100))
        {
            BOOST_CHECK(publisher.publish(zeroeq::make_uint128("Echo"),
                                          echoString.c_str(),
                                          echoString.length()));
            if (monitor.connections == 0 || !received)
            {
                BOOST_CHECK(subscriber.receive(100)); // get second msg (sub
                                                      // or monitor event)
            }
            BOOST_CHECK(received);
            BOOST_CHECK_EQUAL(monitor.connections, 1);
            return;
        }
    }
    BOOST_CHECK(!"reachable");
}

BOOST_AUTO_TEST_CASE(publish_receive_empty_event)
{
    zeroeq::Publisher publisher(zeroeq::NULL_SESSION);
    zeroeq::Subscriber subscriber(publisher.getURI());
    bool received = false;
    BOOST_CHECK(
        subscriber.subscribe(zeroeq::make_uint128("Empty"),
                             zeroeq::EventFunc([&]() { received = true; })));

    for (size_t i = 0; i < 10; ++i)
    {
        BOOST_CHECK(publisher.publish(zeroeq::make_uint128("Empty")));

        if (subscriber.receive(100))
        {
            BOOST_CHECK(received);
            return;
        }
    }
    BOOST_CHECK(!"reachable");
}

BOOST_AUTO_TEST_CASE(two_publishers)
{
    zeroeq::Publisher publisher1(zeroeq::NULL_SESSION);
    zeroeq::Publisher publisher2(zeroeq::NULL_SESSION);
    zeroeq::Subscriber subscriber(
        zeroeq::URIs{publisher1.getURI(), publisher2.getURI()});
    bool received = false;
    BOOST_CHECK(
        subscriber.subscribe(zeroeq::make_uint128("Empty"),
                             zeroeq::EventFunc([&]() { received = true; })));

    for (size_t i = 0; i < 10; ++i)
    {
        BOOST_CHECK(publisher1.publish(zeroeq::make_uint128("Empty")));

        if (subscriber.receive(100))
        {
            BOOST_CHECK(received);
            received = false;
            while (subscriber.receive(100))
                ; /* drain publisher1 events */

            for (i = 0; i < 10; ++i)
            {
                BOOST_CHECK(publisher2.publish(zeroeq::make_uint128("Empty")));

                if (subscriber.receive(100))
                {
                    BOOST_CHECK(received);
                    return;
                }
            }
            BOOST_CHECK(!"reachable");
        }
    }
    BOOST_CHECK(!"reachable");
}

BOOST_AUTO_TEST_CASE(no_receive)
{
    zeroeq::Subscriber subscriber(zeroeq::URI("1.2.3.4:1234"));
    BOOST_CHECK(!subscriber.receive(100));
}

BOOST_AUTO_TEST_CASE(subscribe_to_same_session_zeroconf)
{
    zeroeq::Publisher publisher(zeroeq::TEST_SESSION);
    BOOST_CHECK_NO_THROW(zeroeq::Subscriber subscriber(publisher.getSession()));
}

BOOST_AUTO_TEST_CASE(subscribe_to_different_session_zeroconf)
{
    zeroeq::Publisher publisher(zeroeq::TEST_SESSION);
    BOOST_CHECK_NO_THROW(
        zeroeq::Subscriber subscriber(publisher.getSession() + "bar"));
}

BOOST_AUTO_TEST_CASE(no_receive_zeroconf)
{
    zeroeq::Subscriber subscriber(zeroeq::TEST_SESSION);
    BOOST_CHECK(!subscriber.receive(100));
}

BOOST_AUTO_TEST_CASE(publish_receive_zeroconf)
{
    zeroeq::Publisher publisher(zeroeq::TEST_SESSION);
    zeroeq::Subscriber noSubscriber(publisher.getSession());
    zeroeq::detail::Sender::getUUID() =
        servus::make_UUID(); // different machine
    zeroeq::Subscriber subscriber(publisher.getSession());

    BOOST_CHECK(
        subscriber.subscribe(test::Echo::IDENTIFIER(),
                             zeroeq::EventPayloadFunc(&test::onEchoEvent)));
    BOOST_CHECK(
        noSubscriber.subscribe(test::Echo::IDENTIFIER(),
                               zeroeq::EventPayloadFunc(&test::onEchoEvent)));

    bool received = false;
    for (size_t i = 0; i < 20; ++i)
    {
        BOOST_CHECK(publisher.publish(test::Echo(test::echoMessage)));

        BOOST_CHECK(!noSubscriber.receive(100));
        if (subscriber.receive(10))
        {
            received = true;
            break;
        }
    }
    BOOST_CHECK(received);
}

BOOST_AUTO_TEST_CASE(publish_receive_zeroconf_disabled)
{
    if (getenv("TRAVIS"))
        return;

    zeroeq::Publisher publisher(zeroeq::NULL_SESSION);
    zeroeq::Subscriber subscriber("zeroeq_test_none");

    BOOST_CHECK(
        subscriber.subscribe(test::Echo::IDENTIFIER(),
                             zeroeq::EventPayloadFunc(&test::onEchoEvent)));

    bool received = false;
    for (size_t i = 0; i < 20; ++i)
    {
        BOOST_CHECK(publisher.publish(test::Echo(test::echoMessage)));

        if (subscriber.receive(100))
        {
            received = true;
            break;
        }
    }
    BOOST_CHECK(!received);
}

BOOST_AUTO_TEST_CASE(publish_receive_filters)
{
    zeroeq::Publisher publisher(
        zeroeq::URI("inproc://zeroeq.test.publish_receive_filters"),
        zeroeq::NULL_SESSION);
    zeroeq::Subscriber subscriber(publisher.getURI());

    // Make sure we're connected
    BOOST_CHECK(
        subscriber.subscribe(test::Echo::IDENTIFIER(),
                             zeroeq::EventPayloadFunc(&test::onEchoEvent)));
    for (size_t i = 0; i < 20; ++i)
    {
        BOOST_CHECK(publisher.publish(test::Echo(test::echoMessage)));
        if (subscriber.receive(100))
            break;
    }
    BOOST_CHECK(subscriber.unsubscribe(test::Echo::IDENTIFIER()));

    // benchmark with no data to be transmitted
    auto startTime = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 20000; ++i)
    {
        BOOST_CHECK(publisher.publish(test::Echo()));
        while (subscriber.receive(0)) /* NOP to drain */
            ;
    }
    const auto noEchoTime =
        std::chrono::high_resolution_clock::now() - startTime;

    // Benchmark with echo handler, now should send data
    const std::string message(60000, 'a');
    BOOST_CHECK(subscriber.subscribe(test::Echo::IDENTIFIER(),
                                     zeroeq::EventPayloadFunc(
                                         [&](const void*, const size_t size) {
                                             BOOST_CHECK_EQUAL(size,
                                                               message.size());
                                         })));

    startTime = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 20000; ++i)
    {
        BOOST_CHECK(publisher.publish(test::Echo(message)));
        while (subscriber.receive(0)) /* NOP to drain */
            ;
    }

    const auto& echoTime =
        std::chrono::high_resolution_clock::now() - startTime;

    BOOST_CHECK_MESSAGE(noEchoTime < echoTime,
                        std::chrono::nanoseconds(noEchoTime).count()
                            << ", "
                            << std::chrono::nanoseconds(echoTime).count());
}

BOOST_AUTO_TEST_CASE(publish_receive_late_zeroconf)
{
    zeroeq::Subscriber subscriber(zeroeq::TEST_SESSION);
    zeroeq::detail::Sender::getUUID() =
        servus::make_UUID(); // different machine
    zeroeq::Publisher publisher(subscriber.getSession());

    BOOST_CHECK(
        subscriber.subscribe(test::Echo::IDENTIFIER(),
                             zeroeq::EventPayloadFunc(&test::onEchoEvent)));
    for (size_t i = 0; i < 20; ++i)
    {
        BOOST_CHECK(publisher.publish(test::Echo(test::echoMessage)));

        if (subscriber.receive(100))
            return;
    }
    BOOST_CHECK(!"received");
}

BOOST_AUTO_TEST_CASE(publish_receive_empty_event_zeroconf)
{
    zeroeq::Publisher publisher(zeroeq::TEST_SESSION);
    zeroeq::detail::Sender::getUUID() =
        servus::make_UUID(); // different machine
    zeroeq::Subscriber subscriber(publisher.getSession());

    BOOST_CHECK(subscriber.subscribe(test::Empty::IDENTIFIER(),
                                     zeroeq::EventFunc([] {})));
    for (size_t i = 0; i < 20; ++i)
    {
        BOOST_CHECK(publisher.publish(test::Empty()));

        if (subscriber.receive(100))
            return;
    }
    BOOST_CHECK(!"received");
}

namespace
{
class Publisher
{
public:
    Publisher()
        : running(false)
    {
    }

    void run(const std::string& session)
    {
        zeroeq::Publisher publisher(session);
        running = true;
        size_t i = 0;
        while (running)
        {
            BOOST_CHECK(publisher.publish(test::Echo(test::echoMessage)));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ++i;

            if (i > 300)
                ZEROEQTHROW(
                    std::runtime_error("Publisher giving up after 30s"));
        }
    }

    bool running;
};
}

BOOST_AUTO_TEST_CASE(publish_blocking_receive_zeroconf)
{
    zeroeq::Subscriber subscriber(zeroeq::TEST_SESSION);
    zeroeq::detail::Sender::getUUID() =
        servus::make_UUID(); // different machine

    BOOST_CHECK(
        subscriber.subscribe(test::Echo::IDENTIFIER(),
                             zeroeq::EventPayloadFunc(&test::onEchoEvent)));

    Publisher publisher;
    std::thread thread(
        std::bind(&Publisher::run, &publisher, subscriber.getSession()));

    BOOST_CHECK(subscriber.receive());

    publisher.running = false;
    thread.join();
}
