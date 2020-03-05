
/* Copyright (c) 2017, Human Brain Project
 *                     Stefan.Eilemann@epfl.ch
 */

#define BOOST_TEST_MODULE zeroeq_req_rep

#include "common.h"

#include <servus/servus.h>
#include <servus/uri.h>

#include <atomic>
#include <chrono>
#include <thread>

namespace
{
static const float TIMEOUT = 1000.f; // milliseconds

// Run from a thread: No BOOST_CHECK macros allowed. throw up instead.
template <class R>
bool runOnce(zeroeq::Server& server, const test::Echo& request, const R& reply)
{
    bool handled = false;
    const auto func = [&](const void* data, const size_t size) {
        if (!((data && size) || (!data && !size)))
            throw std::runtime_error("Unexpected handle parameters");
        if (handled)
            throw std::runtime_error("Already handled request");

        if (data)
        {
            test::Echo got;
            got.fromBinary(data, size);
            if (got != request)
                throw std::runtime_error("Request does not match expectation");
        }

        handled = true;
        return zeroeq::ReplyData{R::IDENTIFIER(), reply.toBinary()};
    };

    if (!server.handle(test::Echo::IDENTIFIER(), func) ||
        !server.handle(test::Empty::IDENTIFIER(), func) ||
        server.handle(test::Empty::IDENTIFIER(), func))
    {
        throw std::runtime_error("Handler registration failed");
    }

    if (handled)
        throw std::runtime_error("Already handled a request");

    if (!server.receive(TIMEOUT))
        throw std::runtime_error("No request handled");

    if (!server.remove(test::Echo::IDENTIFIER()) ||
        !server.remove(test::Empty::IDENTIFIER()))
    {
        throw std::runtime_error("Can't remove request handler");
    }
    if (server.remove(test::Echo::IDENTIFIER()))
        throw std::runtime_error("Can remove removed request handler");

    return handled;
}

class EchoThrows : public test::Echo
{
public:
    explicit EchoThrows(const std::string& message)
        : Echo(message)
    {
    }

private:
    Data _toBinary() const final
    {
        throw std::runtime_error("I've had enough!");
        return Data();
    }
};
}

BOOST_AUTO_TEST_CASE(serializable)
{
    test::Echo echo("The quick brown fox");
    const test::Echo reply("Jumped over the lazy dog");

    zeroeq::Server server(zeroeq::NULL_SESSION);
    zeroeq::Client client({server.getURI()});
#if (ZMQ_VERSION >= 40104)
    test::Monitor monitor(server);
#endif

    bool serverHandled = false;
    std::thread thread([&] { serverHandled = runOnce(server, echo, reply); });

    bool handled = false;
    client.request(echo, [&](const zeroeq::uint128_t& type, const void* data,
                             const size_t size) {
        BOOST_CHECK_EQUAL(type, test::Echo::IDENTIFIER());
        BOOST_CHECK(data);
        BOOST_CHECK(!handled);

        test::Echo got;
        got.fromBinary(data, size);
        BOOST_CHECK_EQUAL(got, reply);
        handled = true;
    });

    BOOST_CHECK(!handled);
    BOOST_CHECK(client.receive(TIMEOUT));
    BOOST_CHECK(handled);

#if (ZMQ_VERSION >= 40104)
    BOOST_CHECK_EQUAL(monitor.connections, 0);
    BOOST_CHECK(monitor.receive(TIMEOUT));
    BOOST_CHECK_EQUAL(monitor.connections, 1);
#endif

    thread.join();
    BOOST_CHECK(serverHandled);
}

BOOST_AUTO_TEST_CASE(empty_request_raw)
{
    const test::Echo reply("Jumped over the lazy dog");

    zeroeq::Server server(zeroeq::NULL_SESSION);
    zeroeq::Client client({server.getURI()});
#if (ZMQ_VERSION >= 40104)
    test::Monitor monitor(server, client);
#endif

    bool serverHandled = false;
    std::thread thread([&] { serverHandled = runOnce(server, {}, reply); });

    bool handled = false;
    client.request(test::Echo::IDENTIFIER(), nullptr, 0,
                   [&](const zeroeq::uint128_t& type, const void* data,
                       const size_t size) {
                       BOOST_CHECK_EQUAL(type, test::Echo::IDENTIFIER());
                       BOOST_CHECK(data);
                       BOOST_CHECK(!handled);

                       test::Echo got;
                       got.fromBinary(data, size);
                       BOOST_CHECK_EQUAL(got, reply);
                       handled = true;
                   });

    BOOST_CHECK(!handled);
    BOOST_CHECK(client.receive(TIMEOUT));
#if (ZMQ_VERSION >= 40104)
    if (!handled || monitor.connections == 0)
    {
        BOOST_CHECK(client.receive(TIMEOUT));
    }
    BOOST_CHECK_EQUAL(monitor.connections, 1);
#endif
    BOOST_CHECK(handled);

    thread.join();
    BOOST_CHECK(serverHandled);
}

BOOST_AUTO_TEST_CASE(issue224)
{
    test::Echo echo("The quick brown fox");
    const EchoThrows reply("Jumped over the lazy dog");

    zeroeq::Server server(zeroeq::NULL_SESSION);
    zeroeq::Client client({server.getURI()});

    bool serverHandled = false;
    std::thread thread([&] { serverHandled = runOnce(server, echo, reply); });

    bool handled = false;
    client.request(echo, [&](const zeroeq::uint128_t& type, const void* data,
                             const size_t size) {
        BOOST_CHECK_EQUAL(type, 0);
        BOOST_CHECK(!data);
        BOOST_CHECK_EQUAL(size, 0);
        BOOST_CHECK(!handled);

        handled = true;
    });

    BOOST_CHECK(!handled);
    BOOST_CHECK(client.receive(TIMEOUT));
    BOOST_CHECK(handled);

    thread.join();
    BOOST_CHECK(serverHandled);
}

BOOST_AUTO_TEST_CASE(empty_request_object)
{
    zeroeq::Server server(zeroeq::URI("inproc://zeroeq.test.empty_request_raw"),
                          zeroeq::NULL_SESSION);
    zeroeq::Client client({server.getURI()});
    const test::Echo reply("Jumped over the lazy dog");

    bool serverHandled = false;
    std::thread thread([&] { serverHandled = runOnce(server, {}, reply); });

    bool handled = false;
    client.request(test::Empty(), [&](const zeroeq::uint128_t& type,
                                      const void* data, const size_t size) {
        BOOST_CHECK_EQUAL(type, test::Echo::IDENTIFIER());
        BOOST_CHECK(data);
        BOOST_CHECK(!handled);

        test::Echo got;
        got.fromBinary(data, size);
        BOOST_CHECK_EQUAL(got, reply);
        handled = true;
    });

    BOOST_CHECK(!handled);
    BOOST_CHECK(client.receive(TIMEOUT));
    BOOST_CHECK(handled);

    thread.join();
    BOOST_CHECK(serverHandled);
}

BOOST_AUTO_TEST_CASE(empty_reqrep)
{
    zeroeq::Server server(zeroeq::NULL_SESSION);
    zeroeq::Client client({server.getURI()});
    const test::Empty reply{};

    bool serverHandled = false;
    std::thread thread([&] { serverHandled = runOnce(server, {}, reply); });

    bool handled = false;
    client.request(test::Echo::IDENTIFIER(), nullptr, 0,
                   [&](const zeroeq::uint128_t& type, const void* data,
                       const size_t size) {
                       BOOST_CHECK_EQUAL(type, test::Empty::IDENTIFIER());
                       BOOST_CHECK(!data);
                       BOOST_CHECK_EQUAL(size, 0);
                       BOOST_CHECK(!handled);
                       handled = true;
                   });

    BOOST_CHECK(!handled);
    BOOST_CHECK(client.receive(TIMEOUT));
    BOOST_CHECK(handled);

    thread.join();
    BOOST_CHECK(serverHandled);
}

BOOST_AUTO_TEST_CASE(unhandled_request)
{
    zeroeq::Server server(zeroeq::NULL_SESSION);
    zeroeq::Client client({server.getURI()});
    const test::Empty reply{};

    bool serverHandled = false;
    std::thread thread([&] { serverHandled = runOnce(server, {}, reply); });

    bool handled = false;
    client.request(servus::make_UUID(), nullptr, 0,
                   [&](const zeroeq::uint128_t& type, const void* data,
                       const size_t size) {
                       BOOST_CHECK_EQUAL(type, servus::uint128_t());
                       BOOST_CHECK(!data);
                       BOOST_CHECK_EQUAL(size, 0);
                       BOOST_CHECK(!handled);
                       handled = true;
                   });

    BOOST_CHECK(!handled);
    BOOST_CHECK(client.receive(TIMEOUT));
    BOOST_CHECK(handled);

    thread.join();
    BOOST_CHECK(!serverHandled);
}

BOOST_AUTO_TEST_CASE(two_servers)
{
    test::Echo echo("The quick brown fox");
    const test::Echo reply("Jumped over the lazy dog");

    zeroeq::Server server1(zeroeq::NULL_SESSION);
    zeroeq::Server server2(zeroeq::NULL_SESSION);
    zeroeq::Client client(zeroeq::URIs{server1.getURI(), server2.getURI()});

    bool serverHandled = true;
    std::thread thread1([&] {
        if (!runOnce(server1, echo, reply))
            serverHandled = false;
    });
    std::thread thread2([&] {
        if (!runOnce(server2, echo, reply))
            serverHandled = false;
    });

    size_t handled = 0;
    const auto func = [&](const zeroeq::uint128_t& type, const void* data,
                          const size_t size) {
        BOOST_CHECK_EQUAL(type, test::Echo::IDENTIFIER());
        BOOST_CHECK(data);

        test::Echo got;
        got.fromBinary(data, size);
        BOOST_CHECK_EQUAL(got, reply);
        ++handled;
    };
    client.request(echo, func);
    client.request(echo, func);

    BOOST_CHECK_EQUAL(handled, 0);
    BOOST_CHECK(client.receive(TIMEOUT));
    if (handled < 2)
        BOOST_CHECK(client.receive(TIMEOUT));
    BOOST_CHECK_EQUAL(handled, 2);
    BOOST_CHECK(!client.receive(TIMEOUT / 10));
    BOOST_CHECK_EQUAL(handled, 2);

    thread1.join();
    thread2.join();
    BOOST_CHECK(serverHandled);
}

BOOST_AUTO_TEST_CASE(envconnect)
{
    test::Echo echo("The quick brown fox");
    const test::Echo reply("Jumped over the lazy dog");

    zeroeq::Server server1(zeroeq::NULL_SESSION);
    zeroeq::Server server2(zeroeq::NULL_SESSION);
    std::string servers(server1.getURI().getHost() + ":" +
                        std::to_string(int(server1.getURI().getPort())) + "," +
                        server2.getURI().getHost() + ":" +
                        std::to_string(int(server2.getURI().getPort())));
    setenv("ZEROEQ_SERVERS", servers.c_str(), 1);

    zeroeq::Client client;

    bool handled1 = false;
    bool handled2 = false;
    std::thread thread1([&] { handled1 = runOnce(server1, echo, reply); });
    std::thread thread2([&] { handled2 = runOnce(server2, echo, reply); });

    size_t handled = 0;
    const auto func = [&](const zeroeq::uint128_t&, const void*, size_t) {
        ++handled;
    };
    client.request(echo, func);
    client.request(echo, func);

    BOOST_CHECK_EQUAL(handled, 0);
    BOOST_CHECK(client.receive(TIMEOUT));
    if (handled < 2)
        BOOST_CHECK(client.receive(TIMEOUT));
    BOOST_CHECK_EQUAL(handled, 2);

    thread1.join();
    thread2.join();
    BOOST_CHECK(handled1);
    BOOST_CHECK(handled2);
}

BOOST_AUTO_TEST_CASE(two_clients)
{
    test::Echo echo("The quick brown fox");
    const test::Echo reply("Jumped over the lazy dog");

    zeroeq::Server server(zeroeq::NULL_SESSION);
    zeroeq::Client client1({server.getURI()});
    zeroeq::Client client2({server.getURI()});

    bool serverHandled = false;
    std::thread thread([&] {
        serverHandled =
            runOnce(server, echo, reply) && runOnce(server, echo, reply);
    });

    size_t handled = 0;
    const auto func = [&](const zeroeq::uint128_t& type, const void* data,
                          const size_t size) {
        BOOST_CHECK_EQUAL(type, test::Echo::IDENTIFIER());
        BOOST_CHECK(data);

        test::Echo got;
        got.fromBinary(data, size);
        BOOST_CHECK_EQUAL(got, reply);
        ++handled;
    };

    client1.request(echo, func);
    client2.request(echo, func);

    BOOST_CHECK_EQUAL(handled, 0);
    BOOST_CHECK(client1.receive(TIMEOUT));
    BOOST_CHECK_EQUAL(handled, 1);
    BOOST_CHECK(!client1.receive(TIMEOUT / 10));
    BOOST_CHECK_EQUAL(handled, 1);
    BOOST_CHECK(client2.receive(TIMEOUT));
    BOOST_CHECK_EQUAL(handled, 2);
    BOOST_CHECK(!client2.receive(TIMEOUT / 10));
    BOOST_CHECK_EQUAL(handled, 2);

    thread.join();
    BOOST_CHECK(serverHandled);
}

BOOST_AUTO_TEST_CASE(two_clients_shared)
{
    test::Echo echo("The quick brown fox");
    const test::Echo reply("Jumped over the lazy dog");

    zeroeq::Server server(zeroeq::NULL_SESSION);
    zeroeq::Client client1({server.getURI()});
    zeroeq::Client client2({server.getURI()}, client1);

    bool serverHandled = false;
    std::thread thread([&] {
        serverHandled =
            runOnce(server, echo, reply) && runOnce(server, echo, reply);
    });

    size_t handled = 0;
    const auto func = [&](const zeroeq::uint128_t& type, const void* data,
                          const size_t size) {
        BOOST_CHECK_EQUAL(type, test::Echo::IDENTIFIER());
        BOOST_CHECK(data);

        test::Echo got;
        got.fromBinary(data, size);
        BOOST_CHECK_EQUAL(got, reply);
        ++handled;
    };

    client1.request(echo, func);
    client2.request(echo, func);

    BOOST_CHECK_EQUAL(handled, 0);
    BOOST_CHECK(client1.receive(TIMEOUT));
    if (handled < 2)
        BOOST_CHECK(client1.receive(TIMEOUT));

    BOOST_CHECK_EQUAL(handled, 2);
    BOOST_CHECK(!client1.receive(TIMEOUT / 10));
    BOOST_CHECK_EQUAL(handled, 2);

    thread.join();
    BOOST_CHECK(serverHandled);
}

BOOST_AUTO_TEST_CASE(exceptions)
{
    BOOST_CHECK_THROW(zeroeq::Server(""), std::runtime_error);
    BOOST_CHECK_THROW(zeroeq::Server(zeroeq::URI("141.1.1.1")),
                      std::runtime_error);
}
