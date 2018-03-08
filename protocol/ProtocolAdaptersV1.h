#ifndef SERVERPROTOCOLADAPTER_H
#define SERVERPROTOCOLADAPTER_H

#include <cstdint>

#include "common/Types.h"

class IControllable;
namespace rpc {
class client;
class server;
}

namespace v1 {

class ServerProtocolAdapter
{
public:
    ServerProtocolAdapter(rpc::server& server, IControllable& controllable);

private:
    static float   freqFromProto(uint8_t i);
    static float   gainFromProto(int8_t g);
    static float   qFromProto(uint8_t i);
};

class ClientProtocolAdapter
{
public:
    ClientProtocolAdapter(rpc::client& client);

    void setFilterCount(uint8_t i);
    void setFilterType(uint8_t i, Type    t);
    void setFilterFreq(uint8_t i, uint8_t f);
    void setFilterGain(uint8_t i, float  g);
    void setFilterQ(uint8_t i, uint8_t q);

private:
    static int8_t gainToProto(float g);

    rpc::client& m_client;
};

} // namespace v1

#endif // SERVERPROTOCOLADAPTER_H
