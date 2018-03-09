#include "ZeroconfWrapper.h"

#include <atomic>
#include <iostream>
#include <thread>

#include "microdns.h"

static std::atomic_bool doStop(false);

static bool stop(void *p_cookie)
{
    return doStop;
}

ZeroconfWrapper::ZeroconfWrapper()
{
}

ZeroconfWrapper::ZeroconfWrapper(DiscoverCallback callback)
    : m_callback(callback)
{
    listen(m_callback);
}

ZeroconfWrapper::~ZeroconfWrapper()
{
    doStop = true;
    m_thread.join();

    if (m_mdns_ctx) {
        mdns_destroy(m_mdns_ctx);
        m_mdns_ctx = nullptr;
    }
}

void ZeroconfWrapper::listen(DiscoverCallback callback)
{
    m_callback = callback;

    if (mdns_init(&m_mdns_ctx, MDNS_ADDR_IPV4, MDNS_PORT) < 0)  return;

    m_thread = std::thread([this]() {
        static const char *psz_default_name = "_cornrow._tcp.local";
        const char **ppsz_names = &psz_default_name;
        if (mdns_listen(m_mdns_ctx, ppsz_names, 1, RR_PTR, 10, stop, ZeroconfWrapper::callback, this) < 0) {
            std::cerr << "error starting thread" << std::endl;
        };
    });
}

void ZeroconfWrapper::callback(void *p_cookie, int status, const struct rr_entry *entries)
{
    if (status < 0) return;

    uint32_t address = 0;
    uint16_t port = 0;
    char*    hostname = nullptr;
    auto entry = entries;
    while (entry) {
        switch (entry->type) {
        case RR_A:
            hostname = entry->name;
            address = entry->data.A.addr.s_addr;
            break;
        case RR_SRV:
            port = entry->data.SRV.port;
            break;
        case RR_PTR:
        case RR_TXT:
        case RR_AAAA:
        default:
            break;
        }
        entry = entry->next;
    }

    if (address && port && hostname) {
        auto instance = static_cast<ZeroconfWrapper*>(p_cookie);
        instance->m_callback(hostname, address, port);
    }
}
