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

#pragma once

#include <coro/audio/AlsaSink.h>
#include <coro/audio/AppSource.h>
#include <coro/audio/AudioConverter.h>
#include <coro/audio/SbcDecoder.h>
#include <coro/core/AppSink.h>
#include <common/Types.h>

namespace coro
{
namespace audio
{
class Loudness;
class Peq;
}
}

class CoroPipeline
{
public:
    explicit CoroPipeline();
    ~CoroPipeline();

    void pushBuffer(const coro::audio::AudioConf& conf, coro::audio::AudioBuffer& buffer);

    void setVolume(float volume);
    void setLoudness(uint8_t phon);

    void setPeq(const std::vector<common::Filter>& filters);

    //void setCrossover(const common::Filter& crossover);
    //common::Filter crossover() const;

private:
    coro::audio::AppSource m_appSource;
    coro::audio::SbcDecoder m_sbcDecoder;
    coro::audio::AudioConverter<int16_t, float> m_intToFloat;
    coro::audio::AudioConverter<float, int16_t> m_floatToInt;
    coro::audio::AlsaSink   m_alsaSink;
    coro::core::AppSink     m_appSink;
    coro::audio::Peq*       m_peq;
    coro::audio::Loudness*  m_loudness;
};
