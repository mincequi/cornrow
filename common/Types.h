#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <vector>

enum class Type : uint8_t {
    Invalid = 0,

    Peak = 1,

    Max = 127,
};

struct BiQuad
{
    double b0 = 0.0, b1 = 0.0, b2 = 0.0;
    double a1 = 0.0, a2 = 0.0;
};

extern const std::vector<float> twelfthOctaveBandsTable;
extern const std::vector<float> twentyFourthOctaveBandsTable;
extern const std::vector<float> qTable;

#endif // TYPES_H
