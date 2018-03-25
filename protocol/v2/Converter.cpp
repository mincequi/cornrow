#include "Converter.h"

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
    for (; i < twentyFourthOctaveBandsTable.size(); ++i) {
        if (twentyFourthOctaveBandsTable.at(i+1) >= f) break;
    }

    float c = sqrt(twentyFourthOctaveBandsTable.at(i)*twentyFourthOctaveBandsTable.at(i+1));
    if (f <= c) return i;
    else return i+1;
}

float Converter::freqFromProto(uint8_t i)
{
    if (i >= twentyFourthOctaveBandsTable.size()) return 0.0;
    return twentyFourthOctaveBandsTable[i];
}

} // namespace v2
