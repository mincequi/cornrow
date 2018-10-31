#ifndef PROTOCOLTYPES_H
#define PROTOCOLTYPES_H

#include <string>

#include <rpc/msgpack.hpp>

#include <common/Types.h>

MSGPACK_ADD_ENUM(common::FilterType)

namespace v1 {

enum class Code : uint8_t {
    // General
    Invalid         = 0,    // Not used
    Login,
    SetPassword,

    // General pipeline access
    SetPassthrough  = 16,

    GetInputs,
    GetInput,
    SetInput,
    SetInputFormat,         // Set format of input device

    GetOutputs,
    GetOutput,
    SetOutput,
    SetOutputFormat,        // Set format of output device

    // Filter access (versioned).
    GetPresets      = 64,   // Get all presets
    GetPreset,              // Get preset by name (if empty, get current)
    GetPresetNames,         // Get all preset names
    SetPreset,              // Set/load/activate preset by name
    SavePreset,             // Save preset (if empty, save current settings)
    RenamePreset,
    DeletePreset,

    SetFilterCount  = 80,
    AddFilter,
    RemoveFilter,
    SetFilterType,
    SetFilterFreq,
    SetFilterGain,
    SetFilterQ,

    // Reserved
    User            = 96,

    Max             = 127
};

struct Filter
{
    common::FilterType    type; // max 127
    uint8_t freq; // max 121
    int8_t  gain; // -32 ... 127
    uint8_t q;    // max 55
};

struct Preset
{
    MSGPACK_DEFINE(name, sfilters)

    std::string     name;         // max 31
    std::vector<Filter>  filters;      // max 15
    std::vector<uint32_t>& sfilters = reinterpret_cast<std::vector<uint32_t>&>(filters);
};

} // namespace v1

#endif // PROTOCOLTYPES_H
