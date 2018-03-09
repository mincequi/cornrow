#ifndef ZEROCONFWRAPPER_H
#define ZEROCONFWRAPPER_H

#include <cstdint>
#include <functional>
#include <thread>

class ZeroconfWrapper
{
public:
    // Discover mode
    ZeroconfWrapper();
    using DiscoverCallback = std::function<void(std::string hostname, uint32_t address, uint16_t port)>;
    explicit ZeroconfWrapper(DiscoverCallback callback);
    ~ZeroconfWrapper();

    void listen(DiscoverCallback callback);

private:
    static void callback(void *p_cookie, int status, const struct rr_entry *entries);

    // Discover mode
    DiscoverCallback m_callback = nullptr;
    struct mdns_ctx* m_mdns_ctx = nullptr;
    std::thread      m_thread;
};

#endif // ZEROCONFWRAPPER_H
