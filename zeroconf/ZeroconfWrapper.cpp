#include "ZeroconfWrapper.h"

#include <ifaddrs.h>

#include <thread>

#include "asio/ip/host_name.hpp"

extern "C" {
#include "mdns.h"
#include "mdnsd.h"
}

ZeroconfWrapper::ZeroconfWrapper()
    : m_mdnsd(mdnsd_start())
{
}

ZeroconfWrapper::~ZeroconfWrapper()
{
    if (m_mdnsd) {
        mdnsd_stop(m_mdnsd);
        m_mdnsd = nullptr;
    }
}

bool ZeroconfWrapper::announce(uint16_t port)
{
    if (!m_mdnsd) {
        return false;
    }

    std::string hostname = clmdep_asio::ip::host_name().append(".local");

    struct ifaddrs *ifalist;
    struct ifaddrs *ifa;
    if (getifaddrs(&ifalist) < 0) {
        return false;
    }

    // Look for an ipv4, non-loopback interface to use as the main one.
    for (ifa = ifalist; ifa != NULL; ifa = ifa->ifa_next) {
        // Skip loop-back interfaces
        if (ifa->ifa_flags & IFF_LOOPBACK) continue;

        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            uint32_t ip = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr;
            mdnsd_set_hostname(m_mdnsd, hostname.c_str(), ip); // TTL should be 120 seconds
            break;
        }
    }

    if (!ifa) {
        return false;
    }

    /*
    // Skip the first one, it was already added by set_hostname
    for (ifa = ifa->ifa_next; ifa != NULL; ifa = ifa->ifa_next) {
        // Skip loop-back interfaces
        if (ifa->ifa_flags & IFF_LOOPBACK) continue;

        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            uint32_t ip = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr;
            struct rr_entry *a_e = rr_create_a(create_nlabel(hostname.c_str()), ip); // TTL should be 120 seconds
            mdnsd_add_rr(m_mdnsd, a_e);
        }
    }

    freeifaddrs(ifa);
    */

    struct mdns_service *service = mdnsd_register_svc(m_mdnsd, "Cornrow", "_cornrow._tcp.local", port, NULL, NULL); // TTL should be 75 minutes, i.e. 4500 seconds
    mdns_service_destroy(service);

    return true;
}
