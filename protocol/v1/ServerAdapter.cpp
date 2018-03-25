#include "ServerAdapter.h"

#include <iostream>
#include <string>

#include <rpc/server.h>

#include "Converter.h"
#include "common/IControllable.h"

namespace v1 {

#define FUNC(code) std::string(1, static_cast<uint8_t>(code))

ServerAdapter::ServerAdapter(rpc::server& server, IControllable& c)
{
    std::cerr << "protocol v1" << std::endl;

    // Filter access (versioned).
    server.bind(FUNC(Code::GetPresets),     [this, &c] ()                         { return m_converter.presetsToProto(c.getPresets()); });
    server.bind(FUNC(Code::GetPreset),      [this, &c] (const std::string& name)  { return m_converter.presetToProto(c.getPreset(name)); });
    server.bind(FUNC(Code::SavePreset),     [this, &c] (const Preset& preset)     { c.savePreset(m_converter.presetFromProto(preset)); });

    server.bind(FUNC(Code::GetPresetNames), [this, &c] ()                         { return c.getPresetNames(); });
    server.bind(FUNC(Code::SetPreset),      [this, &c] (const std::string& name)  { c.setPreset(name); });
    server.bind(FUNC(Code::RenamePreset),   [this, &c] (const std::string& oldName, const std::string& newName) { c.renamePreset(oldName, newName); });
    server.bind(FUNC(Code::DeletePreset),   [this, &c] (const std::string& name)  { c.deletePreset(name); });

    server.bind(FUNC(Code::SetFilterCount), [this, &c] (uint8_t i)            { c.setFilterCount(i); });
    server.bind(FUNC(Code::SetFilterType),  [this, &c] (uint8_t i, FilterType t) { c.setFilterType(i, t); });
    server.bind(FUNC(Code::SetFilterFreq),  [this, &c] (uint8_t i, uint8_t f) { c.setFilterFreq(i, m_converter.freqFromProto(f)); });
    server.bind(FUNC(Code::SetFilterGain),  [this, &c] (uint8_t i, int8_t  g) { c.setFilterGain(i, m_converter.gainFromProto(g)); });
    server.bind(FUNC(Code::SetFilterQ),     [this, &c] (uint8_t i, uint8_t q) { c.setFilterQ(i, m_converter.qFromProto(q)); });
}

ServerAdapter::~ServerAdapter()
{
}

#undef FUNC

} // namespace v1
