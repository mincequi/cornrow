/**
 * Copyright © 2014-2015 VideoLabs SAS
 *
 * Author: Jonathan Calmels <jbjcalmels@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "compat.h"
#include "utils.h"

#ifndef HAVE_POLL
int poll(struct pollfd *fds, unsigned nfds, int timeout)
{
    DWORD to = (timeout >= 0) ? (DWORD)timeout : INFINITE;

    if (nfds == 0)
    {    /* WSAWaitForMultipleEvents() does not allow zero events */
        if (SleepEx(to, TRUE))
        {
            errno = EINTR;
            return -1;
        }
        return 0;
    }

    WSAEVENT *evts = malloc(nfds * sizeof (WSAEVENT));
    if (evts == NULL)
        return -1; /* ENOMEM */

    DWORD ret = WSA_WAIT_FAILED;
    for (unsigned i = 0; i < nfds; i++)
    {
        SOCKET fd = fds[i].fd;
        long mask = FD_CLOSE;
        fd_set rdset, wrset, exset;

        FD_ZERO(&rdset);
        FD_ZERO(&wrset);
        FD_ZERO(&exset);
        FD_SET(fd, &exset);

        if (fds[i].events & POLLRDNORM)
        {
            mask |= FD_READ | FD_ACCEPT;
            FD_SET(fd, &rdset);
        }
        if (fds[i].events & POLLWRNORM)
        {
            mask |= FD_WRITE | FD_CONNECT;
            FD_SET(fd, &wrset);
        }
        if (fds[i].events & POLLPRI)
            mask |= FD_OOB;

        fds[i].revents = 0;

        evts[i] = WSACreateEvent();
        if (evts[i] == WSA_INVALID_EVENT)
        {
            while (i > 0)
                WSACloseEvent(evts[--i]);
            free(evts);
            errno = ENOMEM;
            return -1;
        }

        if (WSAEventSelect(fds[i].fd, evts[i], mask)
         && WSAGetLastError() == WSAENOTSOCK)
            fds[i].revents |= POLLNVAL;

        struct timeval tv = { 0, 0 };
        /* By its horrible design, WSAEnumNetworkEvents() only enumerates
         * events that were not already signaled (i.e. it is edge-triggered).
         * WSAPoll() would be better in this respect, but worse in others.
         * So use WSAEnumNetworkEvents() after manually checking for pending
         * events. */
        if (select(0, &rdset, &wrset, &exset, &tv) > 0)
        {
            if (FD_ISSET(fd, &rdset))
                fds[i].revents |= fds[i].events & POLLRDNORM;
            if (FD_ISSET(fd, &wrset))
                fds[i].revents |= fds[i].events & POLLWRNORM;
            if (FD_ISSET(fd, &exset))
                /* To add pain to injury, POLLERR and POLLPRI cannot be
                 * distinguished here. */
                fds[i].revents |= POLLERR | (fds[i].events & POLLPRI);
        }

        if (fds[i].revents != 0 && ret == WSA_WAIT_FAILED)
            ret = WSA_WAIT_EVENT_0 + i;
    }

    if (ret == WSA_WAIT_FAILED)
        ret = WSAWaitForMultipleEvents(nfds, evts, FALSE, to, TRUE);

    unsigned count = 0;
    for (unsigned i = 0; i < nfds; i++)
    {
        WSANETWORKEVENTS ne;

        if (WSAEnumNetworkEvents(fds[i].fd, evts[i], &ne))
            memset(&ne, 0, sizeof (ne));
        WSAEventSelect(fds[i].fd, evts[i], 0);
        WSACloseEvent(evts[i]);

        if (ne.lNetworkEvents & FD_CONNECT)
        {
            fds[i].revents |= POLLWRNORM;
            if (ne.iErrorCode[FD_CONNECT_BIT] != 0)
                fds[i].revents |= POLLERR;
        }
        if (ne.lNetworkEvents & FD_CLOSE)
        {
            fds[i].revents |= (fds[i].events & POLLRDNORM) | POLLHUP;
            if (ne.iErrorCode[FD_CLOSE_BIT] != 0)
                fds[i].revents |= POLLERR;
        }
        if (ne.lNetworkEvents & FD_ACCEPT)
        {
            fds[i].revents |= POLLRDNORM;
            if (ne.iErrorCode[FD_ACCEPT_BIT] != 0)
                fds[i].revents |= POLLERR;
        }
        if (ne.lNetworkEvents & FD_OOB)
        {
            fds[i].revents |= POLLPRI;
            if (ne.iErrorCode[FD_OOB_BIT] != 0)
                fds[i].revents |= POLLERR;
        }
        if (ne.lNetworkEvents & FD_READ)
        {
            fds[i].revents |= POLLRDNORM;
            if (ne.iErrorCode[FD_READ_BIT] != 0)
                fds[i].revents |= POLLERR;
        }
        if (ne.lNetworkEvents & FD_WRITE)
        {
            fds[i].revents |= POLLWRNORM;
            if (ne.iErrorCode[FD_WRITE_BIT] != 0)
                fds[i].revents |= POLLERR;
        }
        count += fds[i].revents != 0;
    }

    free(evts);

    if (count == 0 && ret == WSA_WAIT_IO_COMPLETION)
    {
        errno = EINTR;
        return -1;
    }
    return count;
}

#endif
