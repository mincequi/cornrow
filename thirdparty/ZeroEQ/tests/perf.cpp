
/* Copyright (c) 2016-2017, Human Brain Project
 *                          Stefan.Eilemann@epfl.ch
 */

// Performance test measuring pub-sub and req-rep throughput

#define BOOST_TEST_MODULE zeroeq_perf

#include "common.h"
#include <servus/servus.h>
#include <servus/uri.h>

#include <chrono>
#include <cmath>
#include <thread>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

namespace
{
const size_t msgSize = 1024;
const size_t maxMsgSize = 256 * 1024 * 1024;
const size_t maxServers = 32;
const size_t queueSize = 1024;

const servus::uint128_t typeID = servus::make_uint128("zeroeq::test::Message");

class Message : public servus::Serializable
{
public:
    Message(const size_t size)
        : _bottle(size, char(0xaa))
    {
    }
    std::string getTypeName() const final { return "zeroeq::test::Message"; }
    servus::uint128_t getTypeIdentifier() const final { return typeID; }

private:
    bool _fromBinary(const void* data, const size_t size) final
    {
        _bottle.assign((const char*)data, size);
        return true;
    }

    Data _toBinary() const final
    {
        Data data;
        data.ptr =
            std::shared_ptr<const void>(_bottle.data(), [](const void*) {});
        data.size = _bottle.length();
        return data;
    }

    std::string _bottle;
};

class Publisher
{
public:
    Publisher(const size_t size)
        : message(size)
        , sent(0)
        , running(false)
    {
    }

    void run(zeroeq::Publisher& publisher)
    {
        running = true;
        sent = 0;

        while (running)
        {
            publisher.publish(message);
            ++sent;
        }
    }

    const Message message;
    size_t sent;
    bool running;
};

void runPubSub(const std::string& uri)
{
    zeroeq::Publisher publisher(zeroeq::URI(uri), zeroeq::NULL_SESSION);
    zeroeq::Subscriber subscriber(publisher.getURI());
    {
        // establish subscription
        Message message(4096);
        subscriber.subscribe(message);
        while (true)
        {
            publisher.publish(message);
            if (subscriber.receive(100 /*ms*/))
                break;
        }
        while (subscriber.receive(0)) /* flush pending messages */
            ;
        subscriber.unsubscribe(message);
    }

    std::cout << publisher.getURI().getScheme()
              << " pub-sub: msg size, MB/s, P/s, loss" << std::endl;
    for (size_t i = 1; i <= maxMsgSize; i = i << 1)
    {
        Publisher runner(i);
        Message message(i);
        size_t received = 0;
        auto endTime = high_resolution_clock::now();

        message.registerDeserializedCallback([&] {
            ++received;
            endTime = high_resolution_clock::now();
        });
        subscriber.subscribe(message);

        const auto startTime = high_resolution_clock::now();
        std::thread thread([&] { runner.run(publisher); });

        while (duration_cast<milliseconds>(high_resolution_clock::now() -
                                           startTime)
                   .count() < 500)
        {
            subscriber.receive();
        }
        runner.running = false;
        while (received < runner.sent && subscriber.receive(100.f))
            /* nop */;
        thread.join();
        while (received < runner.sent && subscriber.receive(100.f))
            /* nop */;

        const float seconds =
            float(duration_cast<milliseconds>(endTime - startTime).count()) /
            1000.f;
        const int loss = std::round(float(runner.sent - received) /
                                    float(runner.sent) * 100.f);
        const std::string size =
            (i >= 1024 * 1024 ? std::to_string(i >> 20) + "M"
                              : i >= 1024 ? std::to_string(i >> 10) + "K"
                                          : std::to_string(i));

        subscriber.unsubscribe(message);

        std::cout << size << ", "
                  << float(received * i) / 1024.f / 1024.f / seconds << ", "
                  << float(received) / seconds << ", " << loss << "%"
                  << std::endl;
    }
    std::cout << std::endl;
}
}

BOOST_AUTO_TEST_CASE(pubsub)
{
    runPubSub("127.0.0.1");
}

BOOST_AUTO_TEST_CASE(pubsub_inproc)
{
    runPubSub("inproc://zeroeq.test.pubsub_inproc");
}

namespace
{
class Server
{
public:
    Server(const size_t size)
        : message(size)
        , sent(0)
        , running(true)
    {
    }

    void run(zeroeq::Server& server)
    {
        running = true;
        sent = 0;

        server.handle(message.getTypeIdentifier(),
                      [this](const void*, const size_t) {
                          ++sent;
                          return zeroeq::ReplyData{message.getTypeIdentifier(),
                                                   message.toBinary()};
                      });

        while (running)
            server.receive(100);
    }

    Message message;
    size_t sent;
    bool running;
};

void runReqRep(const std::string& uri)
{
    zeroeq::Server server(zeroeq::URI(uri), zeroeq::NULL_SESSION);
    zeroeq::Client client({server.getURI()});
    std::cout << server.getURI().getScheme()
              << " req-rep: msg size, MB/s, P/s, queue depth, servers"
              << std::endl;

    for (size_t i = 1; i <= maxMsgSize; i = i << 1)
    {
        Server runner(i);
        std::thread thread([&] { runner.run(server); });

        Message message(i);
        size_t received = 0;

        const auto startTime = high_resolution_clock::now();
        while (duration_cast<milliseconds>(high_resolution_clock::now() -
                                           startTime)
                   .count() < 500)
        {
            client.request(typeID, nullptr, 0,
                           [&](const zeroeq::uint128_t&, const void* data,
                               const size_t size) {
                               message.fromBinary(data, size);
                               ++received;
                           });
            client.receive();
        }
        runner.running = false;
        const float seconds =
            float(duration_cast<milliseconds>(high_resolution_clock::now() -
                                              startTime)
                      .count()) /
            1000.f;
        const std::string size =
            (i >= 1024 * 1024 ? std::to_string(i >> 20) + "M"
                              : i >= 1024 ? std::to_string(i >> 10) + "K"
                                          : std::to_string(i));
        std::cout << size << ", "
                  << float(received * i) / 1024.f / 1024.f / seconds << ", "
                  << float(received) / seconds << ", 0, 1" << std::endl;
        thread.join();
        BOOST_CHECK_EQUAL(received, runner.sent);
    }
    std::cout << std::endl;
}
}

BOOST_AUTO_TEST_CASE(reqrep)
{
    runReqRep("127.0.0.1");
}

BOOST_AUTO_TEST_CASE(reqrep_inproc)
{
    runReqRep("inproc://zeroeq.testreqrep_inproc");
}

BOOST_AUTO_TEST_CASE(reqrep_async)
{
    zeroeq::Server server(zeroeq::URI("127.0.0.1"), zeroeq::NULL_SESSION);
    zeroeq::Client client({server.getURI()});
    std::cout << "tcp req-rep: msg size, MB/s, P/s, queue depth, servers"
              << std::endl;

    // async
    for (size_t i = 1; i <= maxMsgSize / msgSize; i = i << 1)
    {
        Server runner(msgSize);
        std::thread thread([&] { runner.run(server); });

        Message message(msgSize);
        size_t sent = 0;
        size_t received = 0;

        const auto startTime = high_resolution_clock::now();
        while (duration_cast<milliseconds>(high_resolution_clock::now() -
                                           startTime)
                   .count() < 500)
        {
            while (sent - received > i)
                client.receive();
            client.request(typeID, nullptr, 0,
                           [&](const zeroeq::uint128_t&, const void* data,
                               const size_t size) {
                               message.fromBinary(data, size);
                               ++received;
                           });
            ++sent;
        }
        while (sent - received > 0)
            client.receive();
        runner.running = false;
        const float seconds =
            float(duration_cast<milliseconds>(high_resolution_clock::now() -
                                              startTime)
                      .count()) /
            1000.f;
        const std::string size =
            (i >= 1024 * 1024 ? std::to_string(i >> 20) + "M"
                              : i >= 1024 ? std::to_string(i >> 10) + "K"
                                          : std::to_string(i));
        std::cout << msgSize / 1024 << "K, "
                  << float(received * msgSize) / 1024.f / 1024.f / seconds
                  << ", " << float(received) / seconds << ", " << size << ", 1"
                  << std::endl;
        thread.join();
        BOOST_CHECK_EQUAL(received, runner.sent);
    }
    std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(reqrep_servers)
{
    std::vector<zeroeq::Server> servers;
    while (servers.size() < maxServers)
        servers.emplace_back(
            zeroeq::Server(zeroeq::URI("127.0.0.1"), zeroeq::NULL_SESSION));

    std::cout << "tcp req-rep: msg size, MB/s, P/s, queue depth, servers"
              << std::endl;
    for (size_t i = 1; i <= maxServers; i = i << 1)
    {
        std::vector<Server> runners(i, {i});
        std::vector<std::thread> threads;
        zeroeq::URIs uris;

        while (threads.size() < i)
        {
            const size_t j = threads.size();
            threads.emplace_back(
                std::thread([&] { runners[j].run(servers[j]); }));
            uris.push_back(servers[j].getURI());
            std::cout << uris.back() << std::endl;
        }

        zeroeq::Client client(uris);

        Message message(msgSize);
        size_t received = 0;
        size_t sent = 0;

        const auto startTime = high_resolution_clock::now();
        while (duration_cast<milliseconds>(high_resolution_clock::now() -
                                           startTime)
                   .count() < 500)
        {
            while (sent - received > queueSize)
                BOOST_REQUIRE(client.receive(1000));

            client.request(typeID, nullptr, 0,
                           [&](const zeroeq::uint128_t&, const void* data,
                               const size_t size) {
                               message.fromBinary(data, size);
                               ++received;
                           });
            ++sent;
        }
        while (sent - received > 0)
            BOOST_REQUIRE(client.receive(1000));

        for (auto& runner : runners)
            runner.running = false;

        const float seconds =
            float(duration_cast<milliseconds>(high_resolution_clock::now() -
                                              startTime)
                      .count()) /
            1000.f;
        const std::string size =
            (i >= 1024 * 1024 ? std::to_string(i >> 20) + "M"
                              : i >= 1024 ? std::to_string(i >> 10) + "K"
                                          : std::to_string(i));
        std::cout << msgSize / 1024 << "K, "
                  << float(received * msgSize) / 1024.f / 1024.f / seconds
                  << ", " << float(received) / seconds << ", " << queueSize
                  << ", " << i << std::endl;
        for (auto& thread : threads)
            thread.join();
    }
    std::cout << std::endl;
}
