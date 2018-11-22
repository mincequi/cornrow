#include "ClientAdapter.h"

#include <cmath>
#include <iostream>

#include <rpc/rpc_error.h>
#include <rpc/server.h>

#include "Converter.h"
#include "common/IControllable.h"

namespace v1 {

#define FUNC(code) std::string(1, static_cast<uint8_t>(code))

ClientAdapter::ClientAdapter(rpc::client& client, ErrorCallback callback)
    : m_client(client),
      m_errorCallback(callback)
{
}

ClientAdapter::~ClientAdapter()
{
}

std::vector<common::Preset> ClientAdapter::getPresets()
{
    return m_converter.presetsFromProto(call(FUNC(Code::GetPresets)).as<std::vector<Preset>>());
}

common::Preset ClientAdapter::getPreset(const std::string& name)
{
    return m_converter.presetFromProto(call(FUNC(Code::GetPreset), name).as<Preset>());
}

std::vector<std::string> ClientAdapter::getPresetNames()
{
    return call(FUNC(Code::GetPresetNames)).as<std::vector<std::string>>();
}

void ClientAdapter::setPreset(const std::string& name)
{
    call(FUNC(Code::SetPreset), name);
}

void ClientAdapter::savePreset(const common::Preset& preset)
{
    call(FUNC(Code::SavePreset), m_converter.presetToProto(preset));
}

void ClientAdapter::renamePreset(const std::string& oldName, const std::string& newName)
{
    call(FUNC(Code::RenamePreset), oldName, newName);
}

void ClientAdapter::deletePreset(const std::string& name)
{
    call(FUNC(Code::DeletePreset), name);
}

void ClientAdapter::setFilterCount(uint8_t i)
{
    call(FUNC(Code::SetFilterCount), i);
}

void ClientAdapter::setFilterType(uint8_t i, common::FilterType t)
{
    call(FUNC(Code::SetFilterType), i, t);
}

void ClientAdapter::setFilterFreq(uint8_t i, uint8_t f)
{
    call(FUNC(Code::SetFilterFreq), i, f);
}

void ClientAdapter::setFilterGain(uint8_t i, float g)
{
    call(FUNC(Code::SetFilterGain), i, m_converter.gainToProto(g));
}

void ClientAdapter::setFilterQ(uint8_t i, uint8_t q)
{
    call(FUNC(Code::SetFilterQ), i, q);
}

template <typename... Args>
RPCLIB_MSGPACK::object_handle ClientAdapter::call(std::string const &func_name, Args&&... args) {
    try {
        return m_client.call(func_name, std::forward<Args>(args)...);
    } catch (rpc::rpc_error &e) {
        std::cout << e.get_function_name() << ": " << e.what() << std::endl;
        using err_t = std::tuple<int, std::string>;
        auto err = e.get_error().as<err_t>();
        std::cout << "[error " << std::get<0>(err) << "]: " << std::get<1>(err) << std::endl;
    } catch (rpc::timeout &e) {
        if (m_errorCallback) m_errorCallback( Error::Timeout, e.what() );
    }

    return RPCLIB_MSGPACK::object_handle();
}

#undef FUNC

} // namespace v1
