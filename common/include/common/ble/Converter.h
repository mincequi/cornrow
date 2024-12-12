#pragma once

#include <common/Types.h>
#include <common/ble/Types.h>

namespace common
{
namespace ble
{

class Converter
{
public:
    Converter();
    virtual ~Converter();

protected:
    virtual uint8_t fToBle(float f) const;
    virtual float fFromBle(uint8_t i) const;

    virtual int8_t gToBle(float g) const;
    virtual float gFromBle(int8_t g) const;

    virtual uint8_t qToBle(float q) const;
    virtual float qFromBle(uint8_t i) const;
};

} // namespace ble
} // namespace common
