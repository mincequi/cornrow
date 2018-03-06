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

#if defined (_WIN32) && !defined(HAVE_INET_NTOP)
const char *
inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
        union {
                struct sockaddr_storage ss;
                struct sockaddr_in      sin;
                struct sockaddr_in6     sin6;
        } u;

        memset(&u, 0, sizeof(u));
        switch (af) {
                case AF_INET:
                        u.sin.sin_family = af;
                        memcpy(&u.sin.sin_addr, src, sizeof(struct in_addr));
                        break;
                case AF_INET6:
                        u.sin6.sin6_family = af;
                        memcpy(&u.sin6.sin6_addr, src, sizeof(struct in6_addr));
                        break;
        }
        if (getnameinfo((const struct sockaddr *) &u.ss, ss_len(&u.ss),
            dst, size, NULL, 0, NI_NUMERICHOST) != 0)
                return (NULL);
        return (dst);
}

int inet_pton (int af, const char *src, void *dst)
{
    unsigned char *b = dst;

    switch (af)
    {
        case AF_INET:
            return sscanf (src, "%hhu.%hhu.%hhu.%hhu",
                           b + 0, b + 1, b + 2, b + 3) == 4;
    }
    errno = EAFNOSUPPORT;
    return -1;
}
#endif // _WIN32 && !inet_ntop
