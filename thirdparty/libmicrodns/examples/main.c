/*
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

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "microdns.h"

volatile sig_atomic_t sigflag = 0;

static void sighandler(int signum)
{
        char s[] = "SIGINT received, exiting ...\n";

        write(fileno(stdout), s, sizeof(s));
        sigflag = 1;
}

static bool stop(void *p_cookie)
{
        return (sigflag ? true : false);
}

static void callback(void *p_cookie, int status, const struct rr_entry *entries)
{
        char err[128];

        if (status < 0) {
                mdns_strerror(status, err, sizeof(err));
                fprintf(stderr, "error: %s\n", err);
                return;
        }
        mdns_entries_print(entries);
}

int main(int i_argc, char *ppsz_argv[])
{
        int r = 0;
        char err[128];
        struct mdns_ctx *ctx;
        const char **ppsz_names;
        int i_nb_names;
        static const char *psz_default_name = "_googlecast._tcp.local";

        if (i_argc > 1)
        {
                ppsz_names = (const char **) &ppsz_argv[1];
                i_nb_names = i_argc - 1;
        }
        else
        {
                ppsz_names = &psz_default_name;
                i_nb_names = 1;
        }
        signal(SIGINT, &sighandler);

        if ((r = mdns_init(&ctx, MDNS_ADDR_IPV4, MDNS_PORT)) < 0)
                goto err;
        if ((r = mdns_listen(ctx, ppsz_names, i_nb_names, RR_PTR, 10, stop,
                             callback, NULL)) < 0)
                goto err;
err:
        if (r < 0) {
                mdns_strerror(r, err, sizeof(err));
                fprintf(stderr, "fatal: %s\n", err);
        }
        mdns_destroy(ctx);
        return (0);
}
