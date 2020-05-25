#pragma once

#include <common/Types.h>
#include <common/ble/Types.h>

namespace common
{

class IAudioConf
{
public:
    virtual ~IAudioConf() {}

    virtual std::vector<common::Filter> filters(common::ble::CharacteristicType group) const = 0;
    virtual void setFilters(common::ble::CharacteristicType group, const std::vector<common::Filter>& filters) = 0;

    virtual std::vector<common::IoInterface> ioCaps() = 0;
    virtual std::vector<common::IoInterface> ioConf() const = 0;

    virtual void setInput(const common::IoInterface& interface) = 0;
    virtual void setOutput(const common::IoInterface& interface) = 0;
};

} // namespace common
