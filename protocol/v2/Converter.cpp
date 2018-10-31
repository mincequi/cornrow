#include "Converter.h"

#include <math.h>

namespace v2
{

Converter::Converter()
{
}

Converter::~Converter()
{
}

uint8_t Converter::freqToProto(float f)
{
    uint8_t i = 0;
    for (; i < common::twentyFourthOctaveBandsTable.size(); ++i) {
        if (common::twentyFourthOctaveBandsTable.at(i+1) >= f) break;
    }

    float c = sqrt(common::twentyFourthOctaveBandsTable.at(i)*common::twentyFourthOctaveBandsTable.at(i+1));
    if (f <= c) return i;
    else return i+1;
}

float Converter::freqFromProto(uint8_t i)
{
    if (i >= common::twentyFourthOctaveBandsTable.size()) return 0.0;
    return common::twentyFourthOctaveBandsTable[i];
}

} // namespace v2
