#pragma once

#include <common/Types.h>

namespace common
{

class IControllable
{
public:
    virtual ~IControllable() {}

    virtual void setPassthrough(bool passthrough)       = 0;

    virtual std::vector<common::Preset> getPresets()            = 0;
    virtual common::Preset getPreset(const std::string& name)   = 0;
    virtual std::vector<std::string> getPresetNames()   = 0;
    virtual void setPreset(const std::string& name)     = 0;
    virtual void savePreset(const common::Preset& preset)       = 0;
    virtual void renamePreset(const std::string& oldName, const std::string& newName)   = 0;
    virtual void deletePreset(const std::string& name)  = 0;

    virtual void setFilterCount(uint8_t i)              = 0;
    virtual void setFilterType(uint8_t i, common::FilterType type)    = 0;
    virtual void setFilterFreq(uint8_t i, float f)      = 0;
    virtual void setFilterGain(uint8_t i, float g)      = 0;
    virtual void setFilterQ(uint8_t i,    float q)      = 0;
};

} // namespace common
