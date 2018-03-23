#ifndef PROTOCOLTYPES_H
#define PROTOCOLTYPES_H

#include <string>

#include <rpc/msgpack.hpp>

#include "common/Types.h"

enum class Code : uint8_t {
    // General
    Invalid         = 0,    // Not used
    Login,
    GetVersion,
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
    SetFilterType,
    SetFilterFreq,
    SetFilterGain,
    SetFilterQ,

    // Reserved
    User            = 96,

    Max             = 127
};

MSGPACK_ADD_ENUM(Type)

struct Filter
{
    MSGPACK_DEFINE(type, freq, gain, q)

    Type    type; // max 127
    uint8_t freq; // max 121
    int8_t  gain; // -32 ... 127
    uint8_t q;    // max 55
};

struct Preset
{
    MSGPACK_DEFINE(name, filters)

    std::string         name;         // max 31
    std::vector<Filter> filters;      // max 15
};

#endif // PROTOCOLTYPES_H
