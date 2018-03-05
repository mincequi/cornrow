/*****************************************************************************
 * This file is part of libmicrodns.
 *
 * Copyright © 2014-2016 VideoLabs SAS
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

#ifndef MICRODNS_RR_H
#define MICRODNS_RR_H

#include <stdint.h>

#if defined (__unix__) || defined (__APPLE__)
# include <arpa/inet.h>
#elif defined(_WIN32)
# include <ws2tcpip.h>
#endif

# ifdef __cplusplus
extern "C" {
# endif

enum rr_type {
        RR_A    = 0x01,
        RR_PTR  = 0x0C,
        RR_TXT  = 0x10,
        RR_AAAA = 0x1C,
        RR_SRV  = 0x21,
};

enum rr_class {
        RR_IN = 0x01,
};

struct rr_data_srv {
        uint16_t priority;
        uint16_t weight;
        uint16_t port;
        char     *target;
};

struct rr_data_txt {
        char txt[256]; // RFC 6762

        struct rr_data_txt *next;
};

struct rr_data_ptr {
        char *domain;
};

struct rr_data_a {
        char   addr_str[INET_ADDRSTRLEN];
        struct in_addr addr;
};

struct rr_data_aaaa {
        char   addr_str[INET6_ADDRSTRLEN];
        struct in6_addr addr;
};

union rr_data {
        struct rr_data_srv  SRV;
        struct rr_data_txt  *TXT;
        struct rr_data_ptr  PTR;
        struct rr_data_a    A;
        struct rr_data_aaaa AAAA;
};

struct rr_entry {
        char     *name;
        uint16_t type;
        uint16_t rr_class : 15;
        uint16_t msbit : 1; // unicast query | cache flush (RFC 6762)

        /* Answers only */
        uint32_t ttl;
        uint16_t data_len;
        union    rr_data data;

        struct rr_entry *next;
};

# ifdef __cplusplus
}
# endif

#endif /* MICRODNS_RR_H */
