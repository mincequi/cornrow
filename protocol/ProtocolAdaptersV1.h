#ifndef SERVERPROTOCOLADAPTER_H
#define SERVERPROTOCOLADAPTER_H

#include <cstdint>
#include <functional>

#include "common/Types.h"

class IControllable;
namespace rpc {
class client;
class server;
}

enum class Error {
    None = 0,
    Timeout = 1
};

using ErrorCallback = std::function<void(Error error, std::string errorString)>;

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
    ClientProtocolAdapter(rpc::client& client, ErrorCallback callback = nullptr);

    void setFilterCount(uint8_t i);
    void setFilterType(uint8_t i, Type    t);
    void setFilterFreq(uint8_t i, uint8_t f);
    void setFilterGain(uint8_t i, float  g);
    void setFilterQ(uint8_t i, uint8_t q);

private:
    static int8_t gainToProto(float g);

    rpc::client& m_client;
    ErrorCallback m_errorCallback = nullptr;
};

} // namespace v1

#endif // SERVERPROTOCOLADAPTER_H
