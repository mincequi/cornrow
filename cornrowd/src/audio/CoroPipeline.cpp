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

#include "CoroPipeline.h"

#include <coro/audio/Loudness.h>
#include <coro/audio/Peq.h>

#include "Converter.h"

using namespace coro::core;

CoroPipeline::CoroPipeline()
{
    m_loudness = new coro::audio::Loudness();
    m_peq = new coro::audio::Peq();

    Node::link(m_appSource, m_sbcDecoder);
    Node::link(m_sbcDecoder, m_intToFloat);
    Node::link(m_intToFloat, *m_peq);
    Node::link(*m_peq, *m_loudness);
    Node::link(*m_loudness, m_floatToInt);
    Node::link(m_floatToInt, m_alsaSink);
}

CoroPipeline::~CoroPipeline()
{
    m_alsaSink.stop();
    delete m_peq;
}

void CoroPipeline::start(const coro::audio::AudioConf& conf)
{
    m_alsaSink.start(conf);
}

void CoroPipeline::stop()
{
    m_alsaSink.stop();
}

void CoroPipeline::pushBuffer(const coro::audio::AudioConf& conf, coro::audio::AudioBuffer& buffer)
{
    m_appSource.process(conf, buffer);
}

void CoroPipeline::setVolume(float volume)
{
    m_loudness->setVolume(volume);
}

void CoroPipeline::setLoudness(uint8_t phon)
{
    m_loudness->setLevel(phon);
}

void CoroPipeline::setPeq(const std::vector<common::Filter>& filters)
{
    m_peq->setFilters(audio::toCoro(filters));
}

/*
void CoroPipeline::setCrossover(const common::Filter& crossover)
{
    m_crossover->setFrequency(crossover.f);

    if (crossover.f == 0.0) {
        constructPipeline(Type::Normal);
    } else {
        constructPipeline(Type::Crossover);
    }
}

void Pipeline::setOutputDevice(const std::string& device)
{
    m_alsaPassthroughSink->set_property("device", Glib::ustring(device));
    m_alsaSink->set_property("device", Glib::ustring(device));
}

common::Filter CoroPipeline::crossover() const
{
    common::Filter crossover;
    crossover.type = m_crossover->frequency() == 0.0f ? common::FilterType::Invalid : common::FilterType::Crossover;
    crossover.f = m_crossover->frequency();

    return crossover;
}
*/
