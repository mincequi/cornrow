#include "Converter.h"

#include <math.h>

namespace v1 {

Converter::Converter()
{
}

Converter::~Converter()
{
}

std::vector<Preset> Converter::presetsToProto(const std::vector<common::Preset>& presets)
{
    std::vector<Preset> ret; ret.reserve(presets.size());
    for (const auto& p : presets) {
        ret.push_back(presetToProto(p));
    }
    return ret;
}

std::vector<common::Preset> Converter::presetsFromProto(const std::vector<Preset>& presets)
{
    std::vector<common::Preset> ret; ret.reserve(presets.size());
    for (const auto& p : presets) {
        ret.push_back(presetFromProto(p));
    }
    return ret;
}

Preset Converter::presetToProto(const common::Preset& preset)
{
    Preset ret;
    ret.name = preset.name;
    ret.filters.reserve(preset.filters.size());
    for (const auto& f : preset.filters) {
        ret.filters.push_back(filterToProto(f));
    }

    return ret;
}

common::Preset Converter::presetFromProto(const Preset& preset)
{
    common::Preset ret;
    ret.name = preset.name;
    ret.filters.reserve(preset.filters.size());
    for (const auto& f : preset.filters) {
        ret.filters.push_back(filterFromProto(f));
    }

    return ret;
}

Filter Converter::filterToProto(const common::Filter& filter)
{
    Filter ret;
    ret.type = filter.type;
    ret.freq = freqToProto(filter.f);
    ret.gain = gainToProto(filter.g);
    ret.q    = qToProto(filter.q);

    return ret;
}

common::Filter Converter::filterFromProto(const Filter& filter)
{
    common::Filter ret;
    ret.type = filter.type;
    ret.f = freqFromProto(filter.freq);
    ret.g = gainFromProto(filter.gain);
    ret.q = qFromProto(filter.q);

    return ret;

}

uint8_t Converter::freqToProto(float f)
{
    uint8_t i = 0;
    for (; i < common::twelfthOctaveBandsTable.size(); ++i) {
        if (common::twelfthOctaveBandsTable.at(i+1) >= f) break;
    }

    float c = sqrt(common::twelfthOctaveBandsTable.at(i)*common::twelfthOctaveBandsTable.at(i+1));
    if (f <= c) return i;
    else return i+1;
}

float Converter::freqFromProto(uint8_t i)
{
    if (i >= common::twelfthOctaveBandsTable.size()) return 0.0;
    return common::twelfthOctaveBandsTable[i];
}

int8_t Converter::gainToProto(float g)
{
    return round(g * -2.0);
}

float Converter::gainFromProto(int8_t g)
{
    return g * -0.5;
}

uint8_t Converter::qToProto(float f)
{
    uint8_t i = 0;
    for (; i < common::qTable.size(); ++i) {
        if (common::qTable.at(i+1) >= f) break;
    }

    float c = sqrt(common::qTable.at(i)*common::qTable.at(i+1));
    if (f <= c) return i;
    else return i+1;
}

float Converter::qFromProto(uint8_t i)
{
    if (i >= common::qTable.size()) return 0.0;
    return common::qTable[i];
}

} // namespace v1
