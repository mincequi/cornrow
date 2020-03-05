
/* Copyright (c) 2014-2017, Human Brain Project
 *                          Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *                          Stefan.Eilemann@epfl.ch
 */

#define NOMINMAX // otherwise std::min/max below don't work on VS

#include "receiver.h"
#include "detail/socket.h"
#include "log.h"

#include <algorithm>
#include <chrono>
#include <deque>
#include <stdexcept>

namespace zeroeq
{
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using std::chrono::nanoseconds;

class Receiver::Impl
{
public:
    void add(::zeroeq::Receiver* receiver) { _shared.push_back(receiver); }
    void remove(::zeroeq::Receiver* receiver)
    {
        _shared.erase(std::remove(_shared.begin(), _shared.end(), receiver),
                      _shared.end());
    }

    bool receive(const uint32_t timeout)
    {
        if (timeout == TIMEOUT_INDEFINITE)
            return _blockingReceive();

        // Never fully block. Give receivers a chance to update, e.g., to check
        // for new connections from zeroconf (#20)
        const uint32_t block = std::min(1000u, timeout / 10);

        const auto startTime = high_resolution_clock::now();
        while (true)
        {
            for (::zeroeq::Receiver* receiver : _shared)
                receiver->update();

            const auto endTime = high_resolution_clock::now();
            const uint32_t elapsed =
                nanoseconds(endTime - startTime).count() / 1000000;
            uint32_t wait = 0;
            if (elapsed < timeout)
                wait = std::min(timeout - uint32_t(elapsed), block);

            if (_receive(wait))
                return true;

            if (elapsed >= timeout)
                return false;
        }
    }

private:
    typedef std::vector<::zeroeq::Receiver*> Receivers;
    typedef Receivers::iterator ReceiversIter;

    Receivers _shared;

    bool _blockingReceive()
    {
        while (true)
        {
            for (::zeroeq::Receiver* receiver : _shared)
                receiver->update();

            // Never fully block. Give receivers a chance to update, e.g., to
            // check for new connections from zeroconf (#20)
            if (_receive(1000))
                return true;
        }
    }

    bool _receive(uint32_t timeout)
    {
        // ZMQ notifications on its sockets is edge-triggered, hence we have
        // to receive all pending POLLIN events to not 'loose' notifications
        // from the socket descriptors (c.f. HTTP server).
        // For reference:
        // https://funcptr.net/2012/09/10/zeromq---edge-triggered-notification
        const auto startTime = high_resolution_clock::now();
        bool haveData = false;
        bool hadData = false;
        do
        {
            std::vector<detail::Socket> sockets;
            std::deque<size_t> intervals;
            for (::zeroeq::Receiver* receiver : _shared)
            {
                const size_t before = sockets.size();
                receiver->addSockets(sockets);
                intervals.push_back(sockets.size() - before);
            }

            const auto remaining = duration_cast<milliseconds>(
                                       high_resolution_clock::now() - startTime)
                                       .count();

            switch (zmq_poll(sockets.data(), int(sockets.size()), remaining))
            {
            case -1: // error
                ZEROEQTHROW(std::runtime_error(std::string("Poll error: ") +
                                               zmq_strerror(zmq_errno())));

            case 0: // timeout; no events signaled during poll
                return hadData;

            default:
            {
                // For each event, find the subscriber which supplied the socket
                // and inform it in case there is data on the socket. We saved
                // #sockets for each subscriber above and track them down here
                // as we iterate over all sockets:
                ReceiversIter i = _shared.begin();
                size_t interval = intervals.front();
                intervals.pop_front();

                // prepare for potential next poll; from now on continue
                // non-blocking to fullfil edge-triggered contract
                haveData = false;
                timeout = 0;

                for (auto& socket : sockets)
                {
                    while (interval == 0 || interval-- == 0)
                    {
                        ++i;
                        interval = intervals.front();
                        intervals.pop_front();
                    }

                    if (socket.revents & ZMQ_POLLIN)
                    {
                        if ((*i)->process(socket))
                        {
                            haveData = true;
                            hadData = true;
                        }
                    }
                }
            }
            }
        } while (haveData && duration_cast<milliseconds>(
                                 high_resolution_clock::now() - startTime)
                                     .count() < timeout);
        return hadData;
    }
};

Receiver::Receiver()
    : _impl(new Receiver::Impl)
{
    _impl->add(this);
}

Receiver::Receiver(Receiver& shared)
    : _impl(shared._impl)
{
    _impl->add(this);
}

Receiver::~Receiver()
{
    _impl->remove(this);
}

Receiver::Receiver(Receiver&&) = default;
Receiver& Receiver::operator=(Receiver&&) = default;

bool Receiver::receive(const uint32_t timeout)
{
    return _impl->receive(timeout);
}

// LCOV_EXCL_START
void Receiver::addConnection(const std::string&)
{
    ZEROEQDONTCALL;
}
// LCOV_EXCL_STOP
}
