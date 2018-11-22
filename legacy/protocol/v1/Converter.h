#ifndef CONVERTER_H
#define CONVERTER_H

#include "Types.h"

namespace v1 {

class Converter
{
public:
    Converter();
    virtual ~Converter();

    virtual std::vector<Preset> presetsToProto(const std::vector<common::Preset>& presets);
    virtual std::vector<common::Preset> presetsFromProto(const std::vector<Preset>& presets);
    virtual Preset presetToProto(const common::Preset& preset);
    virtual common::Preset presetFromProto(const Preset& preset);

    virtual Filter filterToProto(const common::Filter& filter);
    virtual common::Filter filterFromProto(const Filter& filter);

    virtual uint8_t freqToProto(float);
    virtual float freqFromProto(uint8_t i);

    virtual int8_t gainToProto(float g);
    virtual float gainFromProto(int8_t g);

    virtual uint8_t qToProto(float f);
    virtual float qFromProto(uint8_t i);
};

} // namespace v1

#endif // CONVERTER_H
