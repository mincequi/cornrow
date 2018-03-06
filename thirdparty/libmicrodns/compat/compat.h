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

#ifndef MICRODNS_COMPAT_H
#define MICRODNS_COMPAT_H

enum {
        MDNS_STDERR = -1, // standard error
        MDNS_NETERR = -2, // network error
        MDNS_LKPERR = -3, // lookup error
        MDNS_ERROR  = -4, // any runtime error that's not originating from the standard library
};

/*
 * POSIX systems
 */

#if defined (__unix__) || defined (__APPLE__)

# include <unistd.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <sys/types.h>
# include <errno.h>
# include <net/if.h>
# include <poll.h>

extern struct timeval os_deadline;

typedef int sock_t;
# define INVALID_SOCKET -1

static inline int os_init(const char *version) { (void)version; return (0);}
static inline int os_cleanup(void) {return (0);}
static inline int os_close(sock_t s) {return (close(s));}
static inline int os_wouldblock(void) {return (errno == EWOULDBLOCK);}

#endif // __unix__ || (__APPLE__)

/*
 * Windows glue
 */

#if defined (_WIN32)

# include <stdint.h>
# include <winsock2.h>
# include <windows.h>
# include <ws2tcpip.h>
# include <iphlpapi.h>

/* MinGW lacks AI_NUMERICSERV */
# ifndef AI_NUMERICSERV
#  define AI_NUMERICSERV 0x00000008
# endif // !AI_NUMERICSERV

extern uint32_t os_deadline;

typedef SOCKET sock_t;
typedef int socklen_t;

static inline int os_init(const char *version)
{
        WSADATA data;
        uint16_t low, high;

        low = version[0] - '0';
        high = version[2] - '0';
        return (WSAStartup(MAKEWORD(low, high), &data));
}
static inline int os_cleanup(void) {return (WSACleanup());}
static inline int os_close(sock_t s) {return (closesocket(s));}
static inline int os_wouldblock(void) {return (WSAGetLastError() == WSAEWOULDBLOCK);}

# define strerror_r(x, y, z) strerror_s(y, z, x)

#ifndef HAVE_INET_NTOP
extern const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
extern int inet_pton(int af, const char *src, void *dst);
# endif // !inet_ntop

typedef DWORD multicast_if;

#else

# if HAVE_IFADDRS
#include <ifaddrs.h>
typedef struct sockaddr_storage multicast_if;
# else
typedef void* multicast_if;
# endif

#endif // _WIN32

#ifndef HAVE_STRUCT_POLLFD
enum
{
    POLLERR=0x1,
    POLLHUP=0x2,
    POLLNVAL=0x4,
    POLLWRNORM=0x10,
    POLLWRBAND=0x20,
    POLLRDNORM=0x100,
    POLLRDBAND=0x200,
    POLLPRI=0x400,
};
#define POLLIN  (POLLRDNORM|POLLRDBAND)
struct pollfd
{
    int fd;
    unsigned events;
    unsigned revents;
};
#endif

#ifndef HAVE_POLL
int poll(struct pollfd *fds, unsigned nfds, int timeout);
#endif

extern int os_strerror(int, char *, size_t);
extern int os_mcast_join(sock_t, const struct sockaddr_storage *, multicast_if mintf);

#endif /* MICRODNS_COMPAT_H */
