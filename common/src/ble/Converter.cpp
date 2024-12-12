#include "Converter.h"

#include <math.h>

namespace common
{
namespace ble
{

Converter::Converter()
{
}

Converter::~Converter()
{
}

uint8_t Converter::fToBle(float f) const
{
    uint8_t i = 0;
    for (; i < common::frequencyTable.size(); ++i) {
        if (common::frequencyTable.at(i+1) >= f) break;
    }

    float c = sqrt(common::frequencyTable.at(i)*common::frequencyTable.at(i+1));
    if (f <= c) return i;
    else return i+1;
}

float Converter::fFromBle(uint8_t i) const
{
    if (i >= common::frequencyTable.size()) return 0.0;
    return common::frequencyTable[i];
}

int8_t Converter::gToBle(float g) const
{
    return round(g * 2.0);
}

float Converter::gFromBle(int8_t g) const
{
    return g * 0.5;
}

uint8_t Converter::qToBle(float f) const
{
    uint8_t i = 0;
    for (; i < common::qTable.size(); ++i) {
        if (common::qTable.at(i+1) >= f) break;
    }

    float c = sqrt(common::qTable.at(i)*common::qTable.at(i+1));
    if (f <= c) return i;
    else return i+1;
}

float Converter::qFromBle(uint8_t i) const
{
    if (i >= common::qTable.size()) return 0.0;
    return common::qTable[i];
}

} // namespace ble
} // namespace common
