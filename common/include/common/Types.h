#pragma once

#include <cstdint>
#include <set>
#include <string>
#include <vector>

namespace common
{

extern const std::set<int> validSampleRates;

//enum class IoInterfaceType : uint8_t {
enum IoInterfaceType {
    Invalid     = 0,

    Default     = 1, // platform specific (alsa on linux)

    Analog      = 2,
    Spdif       = 3,
    Hdmi        = 4,
    Bluetooth   = 5,
    Airplay     = 6
};

enum class SampleFormat : uint8_t {
    Invalid     = 0,

    I16         = 0x1
};

enum class FilterType : uint8_t {
    Invalid     = 0,

    // Linear filters
    Peak        = 1,
    LowPass,
    HighPass,
    LowShelf,
    HighShelf,
    AllPass,

    // Crossover filters
    CrossoverLr2   = 16,    // Q indicates characteristics, while gain indicates cascading. (Base) order is always 2.
                            // e.g. LR4: q = 0.707, g = 2
                            //      BW:  q = 0.707, g <= 1
                            //      Bessel: q = 0.577, g <= 1
                            //      Chebyshev:  q = 1.0, g <= 1
    CrossoverLr4,

    Subwoofer   = 24,       // LFE Crossover (max. 120 Hz, better 80 Hz)

    // Combined filters
    Loudness    = 32,

    /*
    // Non-linear filters
    Dynamic     = 48,   // Compressor, Expander

    // Reserved
    User        = 64,

    Max         = 127
    */
};

struct Filter
{
    Filter(FilterType _type = FilterType::Invalid, double _f = 0.0, double _g = 0.0, double _q = 0.0) :
        type(_type),
        f(_f),
        g(_g),
        q(_q)
    {}

    FilterType  type = FilterType::Invalid;
    double      f = 0.0;
    double      g = 0.0;
    double      q = 0.0;
};

struct Preset
{
    std::string name;
    std::string meta;
    std::vector<Filter> filters;
};

struct BiQuad
{
    double b0 = 0.0, b1 = 0.0, b2 = 0.0;
    double a1 = 0.0, a2 = 0.0;
};

extern const std::vector<double> frequencyTable;
extern const std::vector<double> qTable;

struct IoInterface
{
    IoInterfaceType type: 4;
    bool            isOutput: 1;
    uint8_t         number: 3; // Number (1...n), not Index(0...n)!

    bool operator==(const IoInterface& other) const;
    bool operator<(const IoInterface& other) const;
};

struct Caps
{
    std::vector<IoInterface>  inputs;
    std::vector<IoInterface>  outputs;
};

} // namespace common
