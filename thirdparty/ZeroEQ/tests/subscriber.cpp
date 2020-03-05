
/* Copyright (c) 2014-2017, Human Brain Project
 *                          Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *                          Stefan.Eilemann@epfl.ch
 */

#define BOOST_TEST_MODULE zeroeq_subscriber

#include "common.h"

#include <servus/servus.h>

BOOST_AUTO_TEST_CASE(construction)
{
    BOOST_CHECK_NO_THROW(zeroeq::Subscriber());
    BOOST_CHECK_NO_THROW(zeroeq::Subscriber subscriber("zeroeq_test_none"));
    BOOST_CHECK_NO_THROW(zeroeq::Subscriber(zeroeq::URI("localhost:1234")));

    zeroeq::Subscriber shared;
    BOOST_CHECK_NO_THROW(zeroeq::Subscriber((zeroeq::Receiver&)shared));
    BOOST_CHECK_NO_THROW(zeroeq::Subscriber("zeroeq_test_none", shared));
    BOOST_CHECK_NO_THROW(
        zeroeq::Subscriber(zeroeq::URI("localhost:1234"), shared));
}

BOOST_AUTO_TEST_CASE(invalid_construction)
{
    BOOST_CHECK_THROW(zeroeq::Subscriber{zeroeq::NULL_SESSION},
                      std::runtime_error);
    BOOST_CHECK_THROW(zeroeq::Subscriber(""), std::runtime_error);
    BOOST_CHECK_THROW(zeroeq::Subscriber(zeroeq::URI("localhost")),
                      std::runtime_error);
    BOOST_CHECK_THROW(zeroeq::Subscriber(
                          zeroeq::URI("deadbeef://badcoffee:1234")),
                      std::runtime_error);

    zeroeq::Subscriber shared;
    BOOST_CHECK_THROW((zeroeq::Subscriber{zeroeq::NULL_SESSION, shared}),
                      std::runtime_error);
    BOOST_CHECK_THROW(zeroeq::Subscriber("", shared), std::runtime_error);
    BOOST_CHECK_THROW(zeroeq::Subscriber(zeroeq::URI("localhost"), shared),
                      std::runtime_error);
}

BOOST_AUTO_TEST_CASE(subscribe)
{
    zeroeq::Subscriber subscriber;
    test::Echo echo;

    BOOST_CHECK(subscriber.subscribe(echo));

    BOOST_CHECK(subscriber.subscribe(zeroeq::make_uint128("Empty"),
                                     zeroeq::EventFunc([]() {})));
    BOOST_CHECK(subscriber.subscribe(zeroeq::make_uint128("Echo"),
                                     zeroeq::EventPayloadFunc(
                                         [](const void*, size_t) {})));
}

BOOST_AUTO_TEST_CASE(unsubscribe)
{
    zeroeq::Subscriber subscriber;

    test::Echo echo;
    BOOST_CHECK(subscriber.subscribe(echo));
    BOOST_CHECK(subscriber.unsubscribe(echo));

    BOOST_CHECK(subscriber.subscribe(zeroeq::make_uint128("Empty"),
                                     zeroeq::EventFunc([]() {})));
    BOOST_CHECK(subscriber.unsubscribe(zeroeq::make_uint128("Empty")));

    BOOST_CHECK(subscriber.subscribe(zeroeq::make_uint128("Echo"),
                                     zeroeq::EventPayloadFunc(
                                         [](const void*, size_t) {})));
    BOOST_CHECK(subscriber.unsubscribe(zeroeq::make_uint128("Echo")));
}

BOOST_AUTO_TEST_CASE(invalid_subscribe)
{
    zeroeq::Subscriber subscriber;

    test::Echo echo;
    BOOST_CHECK(subscriber.subscribe(echo));
    BOOST_CHECK(!subscriber.subscribe(echo));

    BOOST_CHECK(subscriber.subscribe(zeroeq::make_uint128("Echo"),
                                     zeroeq::EventPayloadFunc(
                                         [](const void*, size_t) {})));
    BOOST_CHECK(!subscriber.subscribe(zeroeq::make_uint128("Echo"),
                                      zeroeq::EventPayloadFunc(
                                          [](const void*, size_t) {})));
}

BOOST_AUTO_TEST_CASE(test_invalid_unsubscribe)
{
    zeroeq::Subscriber subscriber;
    test::Echo echo;
    BOOST_CHECK(!subscriber.unsubscribe(echo));
    BOOST_CHECK(subscriber.subscribe(echo));
    BOOST_CHECK(subscriber.unsubscribe(echo));
    BOOST_CHECK(!subscriber.unsubscribe(echo));
}

BOOST_AUTO_TEST_CASE(test_invalid_unsubscribe_different_event_objects)
{
    zeroeq::Subscriber subscriber;

    test::Echo echo;
    test::Empty empty;
    BOOST_CHECK(subscriber.subscribe(echo));
    BOOST_CHECK(!subscriber.unsubscribe(empty));
}

BOOST_AUTO_TEST_CASE(not_implemented_servus)
{
    if (servus::Servus::isAvailable())
        return;

    BOOST_CHECK_THROW(zeroeq::Subscriber subscriber("zeroeq_test_none"),
                      std::runtime_error);
}
