#pragma once

#include <cstdint>
#include <set>
#include <string>
#include <vector>

namespace common
{

extern const std::set<int> validSampleRates;

enum class Source : uint8_t {
    Invalid     = 0,

    Default     = 0x01, // platform specific (alsa on linux)
    Bluetooth   = 0x02
};

enum class SampleFormat : uint8_t {
    Invalid     = 0,

    I16         = 0x1
};

struct Config {
    Config(Source source_, int rate_, SampleFormat format_, std::string watchFilename_ = std::string());

    Source  source = Source::Invalid;
    int     rate = 0;
    SampleFormat format = SampleFormat::Invalid;
    std::string watchFilename;
};

enum class Version : uint8_t {
    Invalid     = 0,

    Version1    = 1,
    Version2    = 2
};

enum class FilterTask : uint8_t {
    Invalid     = 0x0,

    Peq         = 0x1,
    Crossover   = 0x2,
    Loudness    = 0x4
};

enum class FilterType : uint8_t {
    Invalid     = 0,

    // Linear filters
    Peak        = 1,
    LowPass,
    HighPass,
    LowShelf,
    HighShelf,
    //AllPass,

    // Crossover filters
    Crossover   = 16,   // Q indicates characteristics, while gain indicates cascading. (Base) order is always 2.
                        // e.g. LR4: q = 0.707, g = 2
                        //      BW:  q = 0.707, g <= 1
                        //      Bessel: q = 0.577, g <= 1
                        //      Chebyshev:  q = 1.0, g <= 1

    Subwoofer,       // LFE Crossover (max. 120 Hz, better 80 Hz)

    /*
    // Combined filters
    Loudness    = 32,

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

} // namespace common
