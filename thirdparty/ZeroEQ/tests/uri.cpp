
/* Copyright (c) 2015, Human Brain Project
 *                     Daniel.Nachbaur@epfl.ch
 */

#define BOOST_TEST_MODULE zeroeq_uri

#include <zeroeq/uri.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(host_only)
{
    zeroeq::URI hostOnly("host");
    BOOST_CHECK_EQUAL(hostOnly.getHost(), "host");
    BOOST_CHECK_EQUAL(hostOnly.getPort(), 0);
    BOOST_CHECK_EQUAL(hostOnly.getScheme(), "tcp");
}

BOOST_AUTO_TEST_CASE(port_only)
{
    zeroeq::URI portOnly("*:1234");
    BOOST_CHECK_EQUAL(portOnly.getHost(), "*");
    BOOST_CHECK_EQUAL(portOnly.getPort(), 1234);
    BOOST_CHECK_EQUAL(portOnly.getScheme(), "tcp");

    portOnly = zeroeq::URI(":1234");
    BOOST_CHECK_EQUAL(portOnly.getHost(), "*");
    BOOST_CHECK_EQUAL(portOnly.getPort(), 1234);
    BOOST_CHECK_EQUAL(portOnly.getScheme(), "tcp");
}

BOOST_AUTO_TEST_CASE(host_port)
{
    zeroeq::URI hostPort("host:1234");
    BOOST_CHECK_EQUAL(hostPort.getHost(), "host");
    BOOST_CHECK_EQUAL(hostPort.getPort(), 1234);
    BOOST_CHECK_EQUAL(hostPort.getScheme(), "tcp");
}

BOOST_AUTO_TEST_CASE(custom_schema)
{
    zeroeq::URI customSchema("inproc://");
    BOOST_CHECK_EQUAL(customSchema.getHost(), "");
    BOOST_CHECK_EQUAL(customSchema.getPort(), 0);
    BOOST_CHECK_EQUAL(customSchema.getScheme(), "inproc");
}

BOOST_AUTO_TEST_CASE(servus_host_only)
{
    zeroeq::URI hostOnly(servus::URI("host"));
    BOOST_CHECK_EQUAL(hostOnly.getHost(), "");
    BOOST_CHECK_EQUAL(hostOnly.getPath(), "host");
    BOOST_CHECK_EQUAL(hostOnly.getPort(), 0);
    BOOST_CHECK_EQUAL(hostOnly.getScheme(), "tcp");
}

BOOST_AUTO_TEST_CASE(servus_port_only)
{
    zeroeq::URI portOnly(servus::URI("*:1234"));
    BOOST_CHECK_EQUAL(portOnly.getHost(), "");
    BOOST_CHECK_EQUAL(portOnly.getPort(), 0);
    BOOST_CHECK_EQUAL(portOnly.getPath(), "*:1234");
    BOOST_CHECK_EQUAL(portOnly.getScheme(), "tcp");
}

BOOST_AUTO_TEST_CASE(servus_host_port)
{
    zeroeq::URI hostPort(servus::URI("host:1234"));
    BOOST_CHECK_EQUAL(hostPort.getHost(), "");
    BOOST_CHECK_EQUAL(hostPort.getPort(), 0);
    BOOST_CHECK_EQUAL(hostPort.getPath(), "host:1234");
    BOOST_CHECK_EQUAL(hostPort.getScheme(), "tcp");
}

BOOST_AUTO_TEST_CASE(servus_custom_schema)
{
    zeroeq::URI customSchema(servus::URI("inproc://"));
    BOOST_CHECK_EQUAL(customSchema.getHost(), "");
    BOOST_CHECK_EQUAL(customSchema.getPort(), 0);
    BOOST_CHECK_EQUAL(customSchema.getScheme(), "inproc");
}

BOOST_AUTO_TEST_CASE(assign_and_compare)
{
    const zeroeq::URI hostPort("host:1234");
    zeroeq::URI copy;
    copy = hostPort;
    BOOST_CHECK_EQUAL(copy, hostPort);

    copy = servus::URI("inproc://");
    BOOST_CHECK_EQUAL(copy, zeroeq::URI(servus::URI("inproc://")));

    copy = servus::URI("host");
    BOOST_CHECK_EQUAL(copy.getScheme(), "tcp");
    BOOST_CHECK_EQUAL(copy, zeroeq::URI(servus::URI("host")));
    BOOST_CHECK_EQUAL(copy, servus::URI("host"));
    BOOST_CHECK(copy != servus::URI("host:12345"));

    copy = copy;
    BOOST_CHECK(copy != hostPort);
    BOOST_CHECK(copy != zeroeq::URI("host"));
}
