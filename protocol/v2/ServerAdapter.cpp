#include "ServerAdapter.h"

#include <iostream>

#include <rpc/server.h>

#include "Converter.h"
#include "Types.h"
#include "common/IControllable.h"

namespace v2 {

using Code = ::v1::Code;

#define FUNC(code) std::string(1, static_cast<uint8_t>(code))

ServerAdapter::ServerAdapter(rpc::server& server, IControllable& c)
    : v1::ServerAdapter(server, c)
{
    std::cerr << "protocol v2" << std::endl;

    server.override_functors(true);

    server.bind(FUNC(Code::GetPresets),     [this, &c] ()                         { return m_converter.presetsToProto(c.getPresets()); });
    server.bind(FUNC(Code::GetPreset),      [this, &c] (const std::string& name)  { return m_converter.presetToProto(c.getPreset(name)); });
    server.bind(FUNC(Code::SavePreset),     [this, &c] (const Preset& preset)     { c.savePreset(m_converter.presetFromProto(preset)); });

    server.bind(FUNC(Code::SetFilterFreq),  [this, &c] (uint8_t i, uint8_t f) { c.setFilterFreq(i, m_converter.freqFromProto(f)); });
}

} // namespace v2
