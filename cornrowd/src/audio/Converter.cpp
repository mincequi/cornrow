#include "Converter.h"

namespace audio
{

common::FilterType fromGstDsp(GstDsp::FilterType in)
{
    switch (in) {
    case GstDsp::FilterType::Invalid:   return common::FilterType::Invalid;
    case GstDsp::FilterType::Peak:      return common::FilterType::Peak;
    case GstDsp::FilterType::LowPass:   return common::FilterType::LowPass;
    case GstDsp::FilterType::HighPass:  return common::FilterType::HighPass;
    // @TODO(mawe): implement shelving filters
    //case GstDsp::FilterType::LowShelf: return GstDsp::FilterType::Invalid;
    //case GstDsp::FilterType::HighShelf: return GstDsp::FilterType::Invalid;
    }

    return common::FilterType::Invalid;
}

GstDsp::FilterType toGstDsp(common::FilterType in)
{
    switch (in) {
    case common::FilterType::Invalid: return GstDsp::FilterType::Invalid;
    case common::FilterType::Peak: return GstDsp::FilterType::Peak;
    case common::FilterType::LowPass: return GstDsp::FilterType::LowPass;
    case common::FilterType::HighPass: return GstDsp::FilterType::HighPass;
    // @TODO(mawe): implement shelving filters
    case common::FilterType::LowShelf: return GstDsp::FilterType::Invalid;
    case common::FilterType::HighShelf: return GstDsp::FilterType::Invalid;
    case common::FilterType::Crossover: return GstDsp::FilterType::Invalid;
    case common::FilterType::Subwoofer: return GstDsp::FilterType::Invalid;
    }

    return GstDsp::FilterType::Invalid;
}

std::vector<common::Filter> fromGstDsp(const std::vector<GstDsp::Filter>& in)
{
    std::vector<common::Filter> out;
    out.reserve(in.size());

    for (const auto& f : in) {
        out.push_back({fromGstDsp(f.type), f.f, f.g, f.q });
    }

    return out;
}

std::vector<GstDsp::Filter> toGstDsp(const std::vector<common::Filter>& in)
{
    std::vector<GstDsp::Filter> out;
    out.reserve(in.size());

    for (const auto& f : in) {
        out.push_back({toGstDsp(f.type), f.f, f.g, f.q });
    }

    return out;
}

} // namespace v1
