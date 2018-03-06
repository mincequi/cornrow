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

#ifndef MICRODNS_MDNS_H
#define MICRODNS_MDNS_H

/**
 * @file microdns.h
 * @brief main functions to listen and announce services
 */

#include <stdbool.h>

#include "rr.h"

# ifdef __cplusplus
extern "C" {
# endif

struct mdns_ctx;

#define MDNS_PORT        5353
#define MDNS_ADDR_IPV4   "224.0.0.251"
#define MDNS_ADDR_IPV6   "FF02::FB"

#define GET_RCODE(x)    (x&0x000f)
#define SET_RCODE(x,c)  (x|(c&0x000f))

#define GET_OPCODE(x)   (x&0x7800)
#define SET_OPCODE(x,c) (x|(c&0x7800))

enum mdns_hdr_flag {
       FLAG_QR = 1 << 15, // Question/Response
       FLAG_AA = 1 << 10, // Authoritative Answer
       FLAG_TC = 1 <<  9, // Truncated Response
       FLAG_RD = 1 <<  8, // Recursion Desired
       FLAG_RA = 1 <<  7, // Recursion Available
       FLAG_Z  = 1 <<  6, // Reserved
       FLAG_AD = 1 <<  5, // Authentic Data
       FLAG_CD = 1 <<  4, // Checking Disabled
};

struct mdns_hdr {
        uint16_t id;
        uint16_t flags;
        uint16_t num_qn;
        uint16_t num_ans_rr;
        uint16_t num_auth_rr;
        uint16_t num_add_rr;
};

typedef void (*mdns_callback)(void*, int, const struct rr_entry *);

/**
 * \return true if the listener should be stopped
 */
typedef bool (*mdns_stop_func)(void*);

/**
 * @brief Allocates and initialize a new mdns context
 *
 * @param ctx Returns the allocated context for the library [OUT]
 * @param addr Address to listen to
 * @param port Port to listen on
 *
 * @see use mdns_destroy() to clean
 *
 * @return 0 if success, negative in other cases
 */
extern int mdns_init(struct mdns_ctx **ctx, const char *addr, unsigned short port);

/**
 * @brief Destroy an mdns context
 *
 * @param ctx The context created by mdns_init()
 *
 * @return 0 if success, negative in other cases
 */
extern int mdns_destroy(struct mdns_ctx *ctx);


/**
 * @brief Send the entries on the network
 *
 * @param ctx A mdns context created by mdns_init()
 * @param hdr A mdns_hdr header
 * @param entries The entries to send
 *
 * @return 0 if successful, negative in other cases
 */
extern int mdns_entries_send(const struct mdns_ctx *ctx, const struct mdns_hdr *hdr, const struct rr_entry *entries);

/**
 * @brief Print in human form an entry to debug
 *
 * @param entry The entry one wants to debug
 */
extern void mdns_entries_print(const struct rr_entry *);

/**
 * @brief Wrapper around strerror to get strings from errors
 *
 * @param error The error number
 * @param buf The buffer where the string can be written
 * @param n The maximum of characters that can be written inside buf
 *
 * @return 0 if success, negative in other cases
 */
extern int mdns_strerror(int error, char *buf, size_t n);

/**
 * @brief The main listening function for mDNS
 *
 * @param ctx A mdns context created by mdns_init()
 * @param names The list of names of the services you are looking for
 * @param nb_names The number of names in names list
 * @param type The type of Record you want \see rr_type
 * @param interval The refreshing interval to send a probe request (in seconds)
 * @param stop The stop function to stop the discovery
 * @param callback The callback function to receive the entries
 * @param p_cookie user data for the callback
 *
 * @return 0 if success, negative in other cases
 */
extern int mdns_listen(const struct mdns_ctx *ctx, const char *const names[],
                       unsigned int nb_names, enum rr_type type,
                       unsigned int interval, mdns_stop_func stop,
                       mdns_callback callback, void *p_cookie);

/**
 * @brief Announce a new name to serve
 *
 * @param ctx A mdns context created by mdns_init()
 * @param service The name of the services you want to announce
 * @param type The type of Record you want \see rr_type
 * @param callback The callback function to send the entries
 * @param p_cookie user data for the callback
 *
 * @return 0 if success, negative in other cases
 */
extern int mdns_announce(struct mdns_ctx *ctx, const char *service, enum rr_type type,
        mdns_callback callback, void *p_cookie);

/**
 * @brief The main serving function for mDNS
 * mdns_announce() must be called before for each service you want to announce.
 *
 * @param ctx A mdns context created by mdns_init()
 * @param stop The stop function to stop the discovery
 * @param p_cookie user data for the callback
 *
 * @return 0 if success, negative in other cases
 */

extern int mdns_serve(struct mdns_ctx *ctx, mdns_stop_func stop, void *p_cookie);

# ifdef __cplusplus
}
# endif

#endif /* MICRODNS_MDNS_H */
