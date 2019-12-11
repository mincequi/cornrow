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

common::FilterType fromCoro(coro::FilterType in)
{
    switch (in) {
    case coro::FilterType::Invalid:   return common::FilterType::Invalid;
    case coro::FilterType::Peak:      return common::FilterType::Peak;
    case coro::FilterType::LowPass:   return common::FilterType::LowPass;
    case coro::FilterType::HighPass:  return common::FilterType::HighPass;
    case coro::FilterType::LowShelf:  return common::FilterType::LowShelf;
    case coro::FilterType::HighShelf: return common::FilterType::HighShelf;
    case coro::FilterType::AllPass:   return common::FilterType::AllPass;
    default: return common::FilterType::Invalid;
    }

    return common::FilterType::Invalid;
}

coro::FilterType toCoro(common::FilterType in)
{
    switch (in) {
    case common::FilterType::Invalid: return coro::FilterType::Invalid;
    case common::FilterType::Peak: return coro::FilterType::Peak;
    case common::FilterType::LowPass: return coro::FilterType::LowPass;
    case common::FilterType::HighPass: return coro::FilterType::HighPass;
    case common::FilterType::LowShelf: return coro::FilterType::LowShelf;
    case common::FilterType::HighShelf: return coro::FilterType::HighShelf;
    case common::FilterType::CrossoverLr2: return coro::FilterType::Crossover;
    case common::FilterType::CrossoverLr4: return coro::FilterType::Crossover;
    default: return coro::FilterType::Invalid;
    }

    return coro::FilterType::Invalid;
}

std::vector<common::Filter> fromCoro(const std::vector<coro::Filter>& in)
{
    std::vector<common::Filter> out;
    out.reserve(in.size());

    for (const auto& f : in) {
        out.push_back({fromCoro(f.type), f.f, f.g, f.q });
    }

    return out;
}

std::vector<coro::Filter> toCoro(const std::vector<common::Filter>& in)
{
    std::vector<coro::Filter> out;
    out.reserve(in.size());

    for (const auto& f : in) {
        out.push_back({toCoro(f.type), f.f, f.g, f.q });
    }

    return out;
}

} // namespace v1
