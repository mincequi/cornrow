#include "ZeroconfBonjour.h"

#include <dns_sd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "asio/ip/address.hpp"

ZeroconfBonjour::ZeroconfBonjour(const std::string& serviceName)
    : m_serviceName(serviceName)
{
    startBrowsing();
}

ZeroconfBonjour::~ZeroconfBonjour()
{
}

bool ZeroconfBonjour::startBrowsing()
{
    if (m_sdRef) {
        m_lastError = "already discovering";
        return false;
    }

    if (DNSServiceBrowse(&m_sdRef, 0, 0, m_serviceName.c_str(), 0, (DNSServiceBrowseReply)ZeroconfBonjour::onBrowseReply, this) != kDNSServiceErr_NoError) {
        m_lastError = "browse error";
        return false;
    }

    return true;
}

void ZeroconfBonjour::stopBrowsing()
{
    if (m_sdRef) {
        DNSServiceRefDeallocate(m_sdRef);
        m_sdRef = nullptr;
    }
}

bool ZeroconfBonjour::browse()
{
    return process(m_sdRef);
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
    ZeroconfBonjour *self = static_cast<ZeroconfBonjour*>(context);

    if (error != kDNSServiceErr_NoError) {
        self->clear();
        self->m_lastError = "browse reply error";
        return;
    }

    if (flags & kDNSServiceFlagsAdd) {
        self->m_services.insert( { serviceName, {} } );
        self->m_currentService = &(self->m_services.at(serviceName));

        DNSServiceRef sdRef;
        if (DNSServiceResolve(&sdRef, kDNSServiceFlagsForceMulticast, interfaceIndex, serviceName, regtype, domain, ZeroconfBonjour::onResolveReply, self) != kDNSServiceErr_NoError) {
            self->m_lastError = "resolve error";
            return;
        }

        process(sdRef);
        DNSServiceRefDeallocate(sdRef);
    } else {
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
    ZeroconfBonjour *self = static_cast<ZeroconfBonjour*>(context);

    if (error != kDNSServiceErr_NoError || !self->m_currentService) {
        self->m_lastError = "resolve reply error";
        self->m_currentService = nullptr;
        return;
    }

    self->m_currentService->hostname = hostname;
    self->m_currentService->port     = ntohs(port);
    self->m_currentService->txtRecord = std::string((const char*)txtRecord, txtLen);

    DNSServiceRef sdRef;
    if (DNSServiceGetAddrInfo(&sdRef, kDNSServiceFlagsForceMulticast, interfaceIndex, 0, hostname, ZeroconfBonjour::onGetAddrInfoReply, self) != kDNSServiceErr_NoError) {
        self->m_lastError = "get address error";
        return;
    }

    process(sdRef);
    DNSServiceRefDeallocate(sdRef);
}

void ZeroconfBonjour::onGetAddrInfoReply(DNSServiceRef,
                                         DNSServiceFlags flags,
                                         uint32_t interfaceIndex,
                                         DNSServiceErrorType error,
                                         const char *hostname,
                                         const struct sockaddr *address,
                                         uint32_t ttl,
                                         void *context)
{
    ZeroconfBonjour *self = static_cast<ZeroconfBonjour*>(context);

    if (error != kDNSServiceErr_NoError || !self->m_currentService) {
        self->m_lastError = "get address reply error";
        self->m_currentService = nullptr;
        return;
    }

    if (self->m_currentService->hostname != hostname) {
        self->m_lastError = "hostname mismatch";
        self->m_currentService = nullptr;
        return;
    }

    if (flags & kDNSServiceFlagsAdd) {

        if (address->sa_family == AF_INET) {
            self->m_currentService->address = clmdep_asio::ip::address_v4(ntohl(((sockaddr_in*)address)->sin_addr.s_addr)).to_string();
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
        tv.tv_usec = 500 * 1000; // 500 ms

        const int result = select(nfds, &fdSet, 0, 0, &tv);
        if (result <= 0) {
            return false;
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
    DNSServiceRefDeallocate(m_sdRef);
    m_sdRef = nullptr;
    m_serviceName.clear();
    m_services.clear();
    m_currentService = nullptr;
}
