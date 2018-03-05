/*****************************************************************************
 * This file is part of libmicrodns.
 *
 * Copyright © 2014-2015 VideoLabs SAS
 *
 * Author: Jonathan Calmels <jbjcalmels@gmail.com>
 *
 *****************************************************************************
 * libmicrodns is released under LGPLv2.1 (or later) and is also available
 * under a commercial license.
 *****************************************************************************
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef MICRODNS_UTILS_H
#define MICRODNS_UTILS_H

#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

#include "compat.h"

#define MDNS_DN_MAXSZ 256 // domain name maximum size

static inline int ss_family(const struct sockaddr_storage *ss)
{
    return (((const struct sockaddr *) ss)->sa_family);
}

static inline int ss_level(const struct sockaddr_storage *ss)
{
    return (ss_family(ss) == AF_INET ? IPPROTO_IP : IPPROTO_IPV6);
}

static inline socklen_t ss_len(const struct sockaddr_storage *ss)
{
    return (ss_family(ss) == AF_INET ? sizeof(struct sockaddr_in)
                                     : sizeof(struct sockaddr_in6));
}

static inline uint8_t *write_u16(uint8_t *p, const uint16_t v)
{
        *p++ = (v >> 8) & 0xFF;
        *p++ = (v >> 0) & 0xFF;
        return (p);
}

static inline uint8_t *write_u32(uint8_t *p, const uint32_t v)
{
        *p++ = (v >> 24) & 0xFF;
        *p++ = (v >> 16) & 0xFF;
        *p++ = (v >>  8) & 0xFF;
        *p++ = (v >>  0) & 0xFF;
        return (p);
}

static inline uint8_t *write_raw(uint8_t *p, const uint8_t *v)
{
        size_t len;

        len = strlen((const char *) v) + 1;
        memcpy(p, v, len);
        p += len;
        return (p);
}

static inline const uint8_t *read_u16(const uint8_t *p, size_t *s, uint16_t *v)
{
        *v = 0;
        *v |= *p++ << 8;
        *v |= *p++ << 0;
        *s -= 2;
        return (p);
}

static inline const uint8_t *read_u32(const uint8_t *p, size_t *s, uint32_t *v)
{
        *v = 0;
        *v |= *p++ << 24;
        *v |= *p++ << 16;
        *v |= *p++ << 8;
        *v |= *p++ << 0;
        *s -= 4;
        return (p);
}

#endif /* MICRODNS_UTILS_H */
