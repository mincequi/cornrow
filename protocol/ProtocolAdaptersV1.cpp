#include "ProtocolAdaptersV1.h"

#include <cmath>
#include <iostream>

#include <rpc/client.h>
#include <rpc/rpc_error.h>
#include <rpc/server.h>

#include "common/IControllable.h"
#include "common/Types.h"
#include "ProtocolTypesV1.h"

namespace v1 {

#define FUNC(code) std::string(1, static_cast<uint8_t>(code))

ServerProtocolAdapter::ServerProtocolAdapter(rpc::server& server, IControllable& c)
{
    //server.bind(FUNC(Code::GetPresets),     [&c]() { return c.getPresets(); });

    server.bind(FUNC(Code::SetFilterCount), [&c](uint8_t i)            { c.setFilterCount(i); });
    server.bind(FUNC(Code::SetFilterType),  [&c](uint8_t i, Type    t) { c.setFilterType(i, t); });
    server.bind(FUNC(Code::SetFilterFreq),  [&c](uint8_t i, uint8_t f) { c.setFilterFreq(i, freqFromProto(f)); });
    server.bind(FUNC(Code::SetFilterGain),  [&c](uint8_t i, int8_t  g) { c.setFilterGain(i, gainFromProto(g)); });
    server.bind(FUNC(Code::SetFilterQ),     [&c](uint8_t i, uint8_t q) { c.setFilterQ(i, qFromProto(q)); });
}

float ServerProtocolAdapter::freqFromProto(uint8_t i)
{
    if (i >= twelfthOctaveBandsTable.size()) return 0.0;
    return twelfthOctaveBandsTable[i];
}

float ServerProtocolAdapter::gainFromProto(int8_t g)
{
    return g * -0.5;
}

float ServerProtocolAdapter::qFromProto(uint8_t i)
{
    if (i >= qTable.size()) return 0.0;
    return qTable[i];
}

ClientProtocolAdapter::ClientProtocolAdapter(rpc::client& client)
    : m_client(client)
{
}

void ClientProtocolAdapter::setFilterCount(uint8_t i)
{
    try {
        m_client.call(FUNC(Code::SetFilterCount), i);
    } catch (rpc::rpc_error &e) {
        std::cout << e.get_function_name() << ": " << e.what() << std::endl;
        using err_t = std::tuple<int, std::string>;
        auto err = e.get_error().as<err_t>();
        std::cout << "[error " << std::get<0>(err) << "]: " << std::get<1>(err) << std::endl;
    }  catch (rpc::timeout &e) {
        std::cout << e.what() << std::endl;
    }
}

void ClientProtocolAdapter::setFilterType(uint8_t i, Type t)
{
    try {
        m_client.call(FUNC(Code::SetFilterType), i, t);
    } catch (rpc::rpc_error &e) {
        std::cout << e.get_function_name() << ": " << e.what() << std::endl;
        using err_t = std::tuple<int, std::string>;
        auto err = e.get_error().as<err_t>();
        std::cout << "[error " << std::get<0>(err) << "]: " << std::get<1>(err) << std::endl;
    }  catch (rpc::timeout &e) {
        std::cout << e.what() << std::endl;
    }
}

void ClientProtocolAdapter::setFilterFreq(uint8_t i, uint8_t f)
{
    try {
        m_client.call(FUNC(Code::SetFilterFreq), i, f);
    } catch (rpc::rpc_error &e) {
        std::cout << e.get_function_name() << ": " << e.what() << std::endl;
        using err_t = std::tuple<int, std::string>;
        auto err = e.get_error().as<err_t>();
        std::cout << "[error " << std::get<0>(err) << "]: " << std::get<1>(err) << std::endl;
    } catch (rpc::timeout &e) {
        std::cout << e.what() << std::endl;
    }
}

void ClientProtocolAdapter::setFilterGain(uint8_t i, float g)
{
    try {
        m_client.call(FUNC(Code::SetFilterGain), i, gainToProto(g));
    } catch (rpc::rpc_error &e) {
        std::cout << e.get_function_name() << ": " << e.what() << std::endl;
        using err_t = std::tuple<int, std::string>;
        auto err = e.get_error().as<err_t>();
        std::cout << "[error " << std::get<0>(err) << "]: " << std::get<1>(err) << std::endl;
    } catch (rpc::timeout &e) {
        std::cout << e.what() << std::endl;
    }
}

void ClientProtocolAdapter::setFilterQ(uint8_t i, uint8_t q)
{
    try {
        m_client.call(FUNC(Code::SetFilterQ), i, q);
    } catch (rpc::rpc_error &e) {
        std::cout << e.get_function_name() << ": " << e.what() << std::endl;
        using err_t = std::tuple<int, std::string>;
        auto err = e.get_error().as<err_t>();
        std::cout << "[error " << std::get<0>(err) << "]: " << std::get<1>(err) << std::endl;
    } catch (rpc::timeout &e) {
        std::cout << e.what() << std::endl;
    }
}

int8_t ClientProtocolAdapter::gainToProto(float g)
{
    return round(g * -2.0);
}

#undef FUNC

} // namespace v1
