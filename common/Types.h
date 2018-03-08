#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <vector>

enum class Type : uint8_t {
    Invalid = 0,

    Peak = 1,

    Max = 127,
};

extern const std::vector<float> twelfthOctaveBandsTable;
extern const std::vector<float> twentyFourthOctaveBandsTable;
extern const std::vector<float> qTable;

#endif // TYPES_H
