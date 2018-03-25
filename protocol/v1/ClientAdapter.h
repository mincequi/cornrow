#ifndef V1_CLIENTADAPTER_H
#define V1_CLIENTADAPTER_H

#include <functional>
#include <string>

#include <rpc/client.h>

#include "Converter.h"
#include "common/Types.h"

enum class Error {
    None = 0,
    Timeout = 1,
    RpcError = 2,
};

using ErrorCallback = std::function<void(Error error, std::string errorString)>;

namespace v1 {

class ClientAdapter
{
public:
    ClientAdapter(rpc::client& client, ErrorCallback callback = nullptr);
    virtual ~ClientAdapter();

    virtual std::vector<::Preset> getPresets();
    virtual ::Preset getPreset(const std::string& name);
    virtual std::vector<std::string> getPresetNames();
    virtual void setPreset(const std::string& name);
    virtual void savePreset(const ::Preset& preset);
    virtual void renamePreset(const std::string& oldName, const std::string& newName);
    virtual void deletePreset(const std::string& name);

    virtual void setFilterCount(uint8_t i);
    virtual void setFilterType(uint8_t i, FilterType t);
    virtual void setFilterFreq(uint8_t i, uint8_t f);
    virtual void setFilterGain(uint8_t i, float  g);
    virtual void setFilterQ(uint8_t i, uint8_t q);

protected:
    template <typename... Args>
    RPCLIB_MSGPACK::object_handle call(std::string const &func_name, Args&&... args);

    rpc::client& m_client;
    ErrorCallback m_errorCallback = nullptr;
    Converter m_converter;
};

} // namespace v1

#endif // V1_CLIENTADAPTER_H
