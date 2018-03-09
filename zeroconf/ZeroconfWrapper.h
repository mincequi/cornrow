#ifndef TINYSVCMDNS_H
#define TINYSVCMDNS_H

#include <cstdint>
#include <functional>

class ZeroconfWrapper
{
public:
    // Announce mode
    explicit ZeroconfWrapper(uint16_t port);
    ~ZeroconfWrapper();

private:
    // Announce mode
    bool registerService();
    uint16_t m_port = 0;
    struct mdnsd* m_mdnsd = nullptr;

    // Discover mode
};

#endif // TINYSVCMDNS_H
