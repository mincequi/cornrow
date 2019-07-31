/*
 * Copyright (C) 2018 Manuel Weichselbaumer <mincequi@web.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    case GstDsp::FilterType::LowShelf:  return common::FilterType::LowShelf;
    case GstDsp::FilterType::HighShelf: return common::FilterType::HighShelf;
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
    case common::FilterType::LowShelf: return GstDsp::FilterType::LowShelf;
    case common::FilterType::HighShelf: return GstDsp::FilterType::HighShelf;
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
