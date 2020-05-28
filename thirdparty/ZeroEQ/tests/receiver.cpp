
/* Copyright (c) 2014, Human Brain Project
 *                     Stefan.Eilemann@epfl.ch
 */

#define BOOST_TEST_MODULE zeroeq_receiver

#include "common.h"

#include <chrono>

bool gotOne = false;
bool gotTwo = false;

void onEvent1()
{
    gotOne = true;
}
void onEvent2()
{
    gotTwo = true;
}

void testReceive(zeroeq::Publisher& publisher, zeroeq::Receiver& receiver,
                 bool& var1, bool& var2, const int line)
{
    gotOne = false;
    gotTwo = false;

    const auto startTime = std::chrono::high_resolution_clock::now();
    for (;;)
    {
        BOOST_CHECK(publisher.publish(test::Echo(test::echoMessage)));
        while (receiver.receive(100))
        {
        }

        if (var1 && var2)
            break;

        const auto endTime = std::chrono::high_resolution_clock::now();
        const auto elapsed =
            std::chrono::nanoseconds(endTime - startTime).count() / 1000000;
        if (elapsed > 2000 /*ms*/)
            break;
    }
    BOOST_CHECK_MESSAGE(var1, (&var1 == &gotOne ? "Event 1" : "Event 2")
                                  << " not received (l." << line << ")");
    if (&var1 != &var2)
        BOOST_CHECK_MESSAGE(var2, (&var2 == &gotOne ? "Event 1" : "Event 2")
                                      << " not received (l." << line << ")");
}
void testReceive(zeroeq::Publisher& publisher, zeroeq::Receiver& receiver,
                 bool& var, const int line)
{
    testReceive(publisher, receiver, var, var, line);
}

BOOST_AUTO_TEST_CASE(test_two_subscribers)
{
    zeroeq::Publisher publisher(zeroeq::NULL_SESSION);
    zeroeq::Subscriber subscriber1(test::buildURI("localhost", publisher));
    zeroeq::Subscriber subscriber2(test::buildURI("localhost", publisher),
                                   subscriber1);

    BOOST_CHECK(subscriber1.subscribe(test::Echo::IDENTIFIER(),
                                      zeroeq::EventFunc(&onEvent1)));
    BOOST_CHECK(subscriber2.subscribe(test::Echo::IDENTIFIER(),
                                      zeroeq::EventFunc(&onEvent2)));

    testReceive(publisher, subscriber1, gotOne, gotTwo, __LINE__);
    testReceive(publisher, subscriber2, gotOne, gotTwo, __LINE__);
}

BOOST_AUTO_TEST_CASE(test_publisher_routing)
{
    zeroeq::Publisher publisher(zeroeq::NULL_SESSION);
    zeroeq::Publisher silentPublisher(zeroeq::NULL_SESSION);
    zeroeq::Subscriber* subscriber1 =
        new zeroeq::Subscriber(test::buildURI("localhost", silentPublisher));
    zeroeq::Subscriber subscriber2(test::buildURI("localhost", publisher),
                                   *subscriber1);

    BOOST_CHECK(subscriber1->subscribe(test::Echo::IDENTIFIER(),
                                       zeroeq::EventFunc(&onEvent1)));
    BOOST_CHECK(subscriber2.subscribe(test::Echo::IDENTIFIER(),
                                      zeroeq::EventFunc(&onEvent2)));

    testReceive(publisher, *subscriber1, gotTwo, __LINE__);
    BOOST_CHECK(!gotOne);

    testReceive(publisher, subscriber2, gotTwo, __LINE__);
    BOOST_CHECK(!gotOne);

    delete subscriber1;

    testReceive(publisher, subscriber2, gotTwo, __LINE__);
    BOOST_CHECK(!gotOne);
}
