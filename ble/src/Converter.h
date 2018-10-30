#pragma once

#include <common/Types.h>

class QByteArray;

namespace ble
{

class Converter
{
public:
    Converter();
    virtual ~Converter();

    virtual QByteArray filtersToBle(const std::vector<common::Filter>& filters);
    virtual std::vector<common::Filter> filtersFromBle(const QByteArray& array);

protected:
    virtual uint8_t fToBle(float f);
    virtual float fFromBle(uint8_t i);

    virtual int8_t gToBle(float g);
    virtual float gFromBle(int8_t g);

    virtual uint8_t qToBle(float q);
    virtual float qFromBle(uint8_t i);
};

} // namespace ble
