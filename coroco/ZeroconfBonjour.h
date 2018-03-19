#ifndef ZEROCONFBONJOUR_H
#define ZEROCONFBONJOUR_H

#include <dns_sd.h>

#include <map>
#include <string>

class ZeroconfBonjour
{
public:
    ZeroconfBonjour(const std::string& serviceName);
    ~ZeroconfBonjour();

    struct Service {
        std::string hostname;
        std::string address;
        uint16_t    port;
        std::string txtRecord;
    };

    bool startBrowsing();
    void stopBrowsing();
    bool browse();

private:
    static DNSSD_API void onBrowseReply(DNSServiceRef sdRef,
                                        DNSServiceFlags flags,
                                        uint32_t interfaceIndex,
                                        DNSServiceErrorType errorCode,
                                        const char *serviceName,
                                        const char *regtype,
                                        const char *replyDomain,
                                        void       *context);

    static DNSSD_API void onResolveReply(DNSServiceRef sdRef,
                                         DNSServiceFlags flags,
                                         uint32_t interfaceIndex,
                                         DNSServiceErrorType errorCode,
                                         const char *fullname,
                                         const char *hosttarget,
                                         uint16_t port, /* In network byte order */
                                         uint16_t txtLen,
                                         const unsigned char *txtRecord,
                                         void                *context);

    static DNSSD_API void onGetAddrInfoReply(DNSServiceRef sdRef,
                                             DNSServiceFlags flags,
                                             uint32_t interfaceIndex,
                                             DNSServiceErrorType errorCode,
                                             const char *hostname,
                                             const struct sockaddr *address,
                                             uint32_t ttl,
                                             void *context);

    static bool process(DNSServiceRef sdRef);
    void clear();

    DNSServiceRef   m_sdRef = nullptr;
    std::string     m_serviceName;
    std::string     m_lastError;
    std::map<std::string, Service> m_services;
    Service*        m_currentService = nullptr;
};

#endif // ZEROCONFBONJOUR_H
