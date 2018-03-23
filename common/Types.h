#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <vector>

enum class Type : uint8_t {
    Invalid     = 0,

    // Linear filters
    Peak        = 1,
    LowPass,
    HighPass,
    LowShelf,
    HighShelf,
    AllPass,

    Crossover   = 16,   // Q indicates characteristics, while gain indicates cascading. (Base) order is always 2.
                        // e.g. LR4: q = 0.707, g = 2
                        //      BW:  q = 0.707, g <= 1
                        //      Bessel: q = 0.577, g <= 1
                        //      Chebyshev:  q = 1.0, g <= 1
    CrossoverLfe,       // LFE Crossover (max. 120 Hz, better 80 Hz)

    Loudness    = 32,

    // Non-linear filters
    Dynamic     = 64,   // Compressor, Expander

    // Reserved
    User        = 96,

    Max         = 127
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
