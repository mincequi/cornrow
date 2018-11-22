#ifndef TINYSVCMDNS_H
#define TINYSVCMDNS_H

#include <cstdint>
#include <functional>

class ZeroconfWrapper
{
public:
    explicit ZeroconfWrapper();
    ~ZeroconfWrapper();

    bool announce(uint16_t port);

private:
    struct mdnsd* m_mdnsd = nullptr;
};

#endif // TINYSVCMDNS_H
