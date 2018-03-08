#ifndef PROTOCOLTYPES_H
#define PROTOCOLTYPES_H

#include <string>

#include <rpc/msgpack.hpp>

#include "common/Types.h"

enum class Code : uint8_t {
    // Generic
    Invalid,            //
    Login,              //
    GetVersion,

    SetPassthrough,

    // Versioned
    GetPresets,         // Get all presets
    GetPreset,          // Get preset by name (if empty, get current)
    GetPresetNames,     // Get all preset names
    GetCurrentPreset,   // Get current preset
    LoadPreset,         // Load preset by name
    SavePreset,         // Save preset based on current settings.
    RenamePreset,
    DeletePreset,

    SetFilterCount,
    SetFilterType,
    SetFilterFreq,
    SetFilterGain,
    SetFilterQ,

    GetAudioOutputs,
    GetAudioOutput,
    SetAudioOutput,

    GetAudioInputs,
    GetAudioInput,
    SetAudioInput,
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
