#ifndef ZEROCONFBONJOUR_H
#define ZEROCONFBONJOUR_H

#include <dns_sd.h>

#include <functional>
#include <map>
#include <string>
#include <thread>

class ZeroconfBonjour
{
public:
    ZeroconfBonjour();
    ~ZeroconfBonjour();

    struct Service {
        std::string hostname;
        std::string address;
        uint16_t    port;
        std::string txtRecord;
    };

    using DiscoverCallback = std::function<void(Service service)>;
    bool discover(const std::string& serviceName, DiscoverCallback callback);
    void stop();

private:
    static void onBrowseReply(DNSServiceRef sdRef,
                                        DNSServiceFlags flags,
                                        uint32_t interfaceIndex,
                                        DNSServiceErrorType errorCode,
                                        const char *serviceName,
                                        const char *regtype,
                                        const char *replyDomain,
                                        void       *context);

    static void onResolveReply(DNSServiceRef sdRef,
                                         DNSServiceFlags flags,
                                         uint32_t interfaceIndex,
                                         DNSServiceErrorType errorCode,
                                         const char *fullname,
                                         const char *hosttarget,
                                         uint16_t port, /* In network byte order */
                                         uint16_t txtLen,
                                         const unsigned char *txtRecord,
                                         void                *context);

    static void onGetAddrInfoReply(DNSServiceRef sdRef,
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
    // When we resolve, we have to track the current service
    Service*        m_currentService = nullptr;

    DiscoverCallback m_callback = nullptr;
    std::thread      m_thread;
};

#endif // ZEROCONFBONJOUR_H
