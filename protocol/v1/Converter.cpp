#include "Converter.h"

namespace v1 {

Converter::Converter()
{
}

Converter::~Converter()
{
}

std::vector<Preset> Converter::presetsToProto(const std::vector<::Preset>& presets)
{
    std::vector<Preset> ret; ret.reserve(presets.size());
    for (const auto& p : presets) {
        ret.push_back(presetToProto(p));
    }
    return ret;
}

std::vector<::Preset> Converter::presetsFromProto(const std::vector<Preset>& presets)
{
    std::vector<::Preset> ret; ret.reserve(presets.size());
    for (const auto& p : presets) {
        ret.push_back(presetFromProto(p));
    }
    return ret;
}

Preset Converter::presetToProto(const ::Preset& preset)
{
    Preset ret;
    ret.name = preset.name;
    ret.filters.reserve(preset.filters.size());
    for (const auto& f : preset.filters) {
        ret.filters.push_back(filterToProto(f));
    }

    return ret;
}

::Preset Converter::presetFromProto(const Preset& preset)
{
    ::Preset ret;
    ret.name = preset.name;
    ret.filters.reserve(preset.filters.size());
    for (const auto& f : preset.filters) {
        ret.filters.push_back(filterFromProto(f));
    }

    return ret;
}

Filter Converter::filterToProto(const ::Filter& filter)
{
    Filter ret;
    ret.type = filter.type;
    ret.freq = freqToProto(filter.f);
    ret.gain = gainToProto(filter.g);
    ret.q    = qToProto(filter.q);

    return ret;
}

::Filter Converter::filterFromProto(const Filter& filter)
{
    ::Filter ret;
    ret.type = filter.type;
    ret.f = freqFromProto(filter.freq);
    ret.g = gainFromProto(filter.gain);
    ret.q = qFromProto(filter.q);

    return ret;

}

uint8_t Converter::freqToProto(float f)
{
    uint8_t i = 0;
    for (; i < twelfthOctaveBandsTable.size(); ++i) {
        if (twelfthOctaveBandsTable.at(i+1) >= f) break;
    }

    float c = sqrt(twelfthOctaveBandsTable.at(i)*twelfthOctaveBandsTable.at(i+1));
    if (f <= c) return i;
    else return i+1;
}

float Converter::freqFromProto(uint8_t i)
{
    if (i >= twelfthOctaveBandsTable.size()) return 0.0;
    return twelfthOctaveBandsTable[i];
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
    for (; i < qTable.size(); ++i) {
        if (qTable.at(i+1) >= f) break;
    }

    float c = sqrt(qTable.at(i)*qTable.at(i+1));
    if (f <= c) return i;
    else return i+1;
}

float Converter::qFromProto(uint8_t i)
{
    if (i >= qTable.size()) return 0.0;
    return qTable[i];
}

} // namespace v1
