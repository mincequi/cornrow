/*
 * Copyright © 2014-2015 VideoLabs SAS
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
#include "compat.h"

volatile sig_atomic_t sigflag = 0;

void sighandler(int signum)
{
        char s[] = "SIGINT received, exiting ...\n";

        write(fileno(stdout), s, sizeof(s));
        sigflag = 1;
}

bool stop(void *cbarg)
{
        return (sigflag ? true : false);
}

void callback(void *cbarg, int r, const struct rr_entry *entry)
{
        struct mdns_ctx *ctx = (struct mdns_ctx *) cbarg;
        struct mdns_hdr hdr = {0};
        struct rr_entry answer = {0};

        hdr.flags |= FLAG_QR;
        hdr.flags |= FLAG_AA;
        hdr.num_ans_rr = 1;

        answer.name     = entry->name;
        answer.type     = entry->type;
        answer.rr_class = entry->rr_class;
        answer.ttl      = 120;

        sprintf(answer.data.A.addr_str, "192.168.1.1");
        inet_pton(AF_INET, answer.data.A.addr_str, &answer.data.A.addr);
        mdns_entries_send(ctx, &hdr, &answer);
}

int main(int argc, char *argv[])
{
        int r = 0;
        char err[128];
        struct mdns_ctx *ctx;

        signal(SIGINT, sighandler);
        signal(SIGTERM, sighandler);

        if ((r = mdns_init(&ctx, MDNS_ADDR_IPV4, MDNS_PORT)) < 0)
                goto err;

        // test with `ping mdnshost.local`
        mdns_announce(ctx, "mdnshost.local", RR_A, callback, ctx);

        if ((r = mdns_serve(ctx, stop, NULL)) < 0)
                goto err;
err:
        if (r < 0) {
                mdns_strerror(r, err, sizeof(err));
                fprintf(stderr, "fatal: %s\n", err);
        }
        mdns_destroy(ctx);
        return (0);
}
