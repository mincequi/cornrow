#include "ZeroconfBonjour.h"

#include <atomic>
#include <dns_sd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "asio/ip/address.hpp"

#define self static_cast<ZeroconfBonjour*>(context)

static std::atomic_bool doRun(true);

ZeroconfBonjour::ZeroconfBonjour()
{
}

ZeroconfBonjour::~ZeroconfBonjour()
{
    stop();
}

bool ZeroconfBonjour::discover(const std::string& serviceName, ZeroconfBonjour::DiscoverCallback callback)
{
    if (m_sdRef) {
        m_lastError = "already discovering";
        return false;
    }

    if (DNSServiceBrowse(&m_sdRef, 0, 0, serviceName.c_str(), 0, (DNSServiceBrowseReply)ZeroconfBonjour::onBrowseReply, this) != kDNSServiceErr_NoError) {
        m_lastError = "browse error";
        return false;
    }

    m_callback = callback;

    m_thread = std::thread([this]() {
        while (doRun && process(m_sdRef)) {
            ;
        }
    });

    return true;
}

void ZeroconfBonjour::stop()
{
    doRun = false;
    if (m_thread.joinable()) m_thread.join();

    clear();
}

void ZeroconfBonjour::onBrowseReply(DNSServiceRef,
                                    DNSServiceFlags flags,
                                    uint32_t interfaceIndex,
                                    DNSServiceErrorType error,
                                    const char *serviceName,
                                    const char *regtype,
                                    const char *domain,
                                    void       *context)
{
    if (error != kDNSServiceErr_NoError) {
        self->clear();
        self->m_lastError = "browse reply error";
        return;
    }

    if (flags & kDNSServiceFlagsAdd) {
        self->m_services.insert( { serviceName, {} } );

        DNSServiceRef sdRef;
        if (DNSServiceResolve(&sdRef, kDNSServiceFlagsForceMulticast, interfaceIndex, serviceName, regtype, domain, ZeroconfBonjour::onResolveReply, self) != kDNSServiceErr_NoError) {
            self->m_lastError = "resolve error";
            return;
        }

        // When we start resolving, we have to point to current service to be resolved
        self->m_currentService = &(self->m_services.at(serviceName));

        process(sdRef);
        DNSServiceRefDeallocate(sdRef);
    } else {    // An enumeration callback with the "Add" flag NOT set indicates a "Remove"
        self->m_services.erase(serviceName);
        self->m_currentService = nullptr;
    }
}

void ZeroconfBonjour::onResolveReply(DNSServiceRef,
                                     DNSServiceFlags,
                                     uint32_t interfaceIndex,
                                     DNSServiceErrorType error,
                                     const char* /*fullname*/,
                                     const char* hostname,
                                     uint16_t port, /* In network byte order */
                                     uint16_t txtLen,
                                     const unsigned char *txtRecord,
                                     void                *context)
{
    if (error != kDNSServiceErr_NoError) {
        self->m_lastError = "resolve reply error";
        return;
    }

    if (!self->m_currentService) {
        self->m_lastError = "resolve error: no current service";
        return;
    }

    self->m_currentService->hostname = hostname;
    self->m_currentService->port     = ntohs(port);
    self->m_currentService->txtRecord = std::string((const char*)txtRecord, txtLen);
    self->m_currentService = nullptr;

#ifndef __linux__
    // Avahi's Bonjour compatibility layer does not support DNSServiceGetAddrInfo
    DNSServiceRef sdRef;
    if (DNSServiceGetAddrInfo(&sdRef, kDNSServiceFlagsForceMulticast, interfaceIndex, 0, hostname, ZeroconfBonjour::onGetAddrInfoReply, self) != kDNSServiceErr_NoError) {
        self->m_lastError = "get address error";
        return;
    }
    process(sdRef);
    DNSServiceRefDeallocate(sdRef);
#endif
}

void ZeroconfBonjour::onGetAddrInfoReply(DNSServiceRef,
                                         DNSServiceFlags flags,
                                         uint32_t /*interfaceIndex*/,
                                         DNSServiceErrorType error,
                                         const char *hostname,
                                         const struct sockaddr *address,
                                         uint32_t /*ttl*/,
                                         void *context)
{
    if (error != kDNSServiceErr_NoError) {
        self->m_lastError = "get address reply error";
        return;
    }

    if (flags & kDNSServiceFlagsAdd) {
        for (auto& kv : self->m_services) {
            if (kv.second.hostname == hostname) {
                if (address->sa_family == AF_INET) {
                    kv.second.address = clmdep_asio::ip::address_v4(ntohl(((sockaddr_in*)address)->sin_addr.s_addr)).to_string();
                    self->m_callback(kv.second);
                    doRun = false;
                }
            }
        }
    }
}

bool ZeroconfBonjour::process(DNSServiceRef sdRef)
{
    if (!sdRef) return false;

    const int fd = DNSServiceRefSockFD(sdRef);
    if (fd < 0)
        return false;

    const int nfds = fd + 1;
    fd_set fdSet;

    while (true)
    {
        FD_ZERO(&fdSet);
        FD_SET(fd, &fdSet);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100 * 1000; // 100 ms

        const int result = select(nfds, &fdSet, 0, 0, &tv);
        if (result < 0) {
            return false;
        } else if (result == 0) {
            return true;
        }

        if (FD_ISSET(fd, &fdSet))
        {
            if (DNSServiceProcessResult(sdRef) != kDNSServiceErr_NoError) {
                return false;
            }
        }
    }
}

void ZeroconfBonjour::clear()
{
    if (m_sdRef) {
        DNSServiceRefDeallocate(m_sdRef);
        m_sdRef = nullptr;
    }
    m_lastError.clear();
    m_services.clear();
    m_currentService = nullptr;
    m_callback = nullptr;
    m_thread = std::thread();
    doRun = true;
}
