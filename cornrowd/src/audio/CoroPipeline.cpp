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
#include <loguru/loguru.hpp>

#include "Converter.h"

using namespace std::placeholders;
using namespace coro::core;

CoroPipeline::CoroPipeline()
    : m_ac3Encoder(coro::audio::AudioCodec::Ac3)
{
    m_loudness = new coro::audio::Loudness();
    m_peq = new coro::audio::Peq();

    //m_alsaSink.setDevice("iec958:CARD=sndrpihifiberry,DEV=0");

    // Bluetooth nodes
    Node::link(m_appSource, m_sbcDecoder);
    Node::link(m_sbcDecoder, m_intToFloat);

    // Scream nodes
    Node::link(m_screamSource, m_intToFloat);

    Node::link(m_intToFloat, *m_peq);
    Node::link(*m_peq, *m_loudness);
    Node::link(*m_loudness, m_crossover);
    Node::link(m_crossover, m_ac3Encoder);
    Node::link(m_ac3Encoder, m_floatToInt);
    Node::link(m_floatToInt, m_alsaSink);

    m_screamSource.setWantsToStartCallback(std::bind(&CoroPipeline::onSourceWantsToStart, this, _1, _2));

    m_sources.insert(&m_appSource);
    m_sources.insert(&m_screamSource);
}

CoroPipeline::~CoroPipeline()
{
    m_alsaSink.stop();
    delete m_peq;
    delete m_loudness;
}

void CoroPipeline::start(const coro::audio::AudioConf& conf)
{
    LOG_F(INFO, "Start Bluetooth source");
    m_appSource.setWantsToStart(true);
    onSourceWantsToStart(&m_appSource, true);
    m_alsaSink.start(conf);
}

void CoroPipeline::stop()
{
    LOG_F(INFO, "Stop Bluetooth source");
    m_appSource.stop();
    onSourceWantsToStart(&m_appSource, false);
    m_alsaSink.stop();
}

void CoroPipeline::pushBuffer(const coro::audio::AudioConf& conf, coro::audio::AudioBuffer& buffer)
{
    if (!m_appSource.isStarted()) {
        m_appSource.setWantsToStart(true);
        if (!m_appSource.isStarted()) {
            LOG_F(2, "%s not started. Will drop buffer.", m_appSource.name());
            return;
        }
    }

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
    m_peq->setFilters(::audio::toCoro(filters));
}

void CoroPipeline::setCrossover(const common::Filter& filter)
{
    auto crossover = ::audio::toCoro({filter}).front();
    m_crossover.setFilter(crossover);
    m_ac3Encoder.setIsBypassed(!crossover.isValid());
    m_floatToInt.setIsBypassed(crossover.isValid());
}

void CoroPipeline::setOutputDevice(const std::string& device)
{
    m_alsaSink.setDevice(device);
}

/*
common::Filter CoroPipeline::crossover() const
{
    common::Filter crossover;
    crossover.type = m_crossover->frequency() == 0.0f ? common::FilterType::Invalid : common::FilterType::Crossover;
    crossover.f = m_crossover->frequency();

    return crossover;
}
*/

void CoroPipeline::onSourceWantsToStart(coro::audio::Source* const source, bool wantsToStart)
{
    // If source wants to stop, stop it.
    if (!wantsToStart) {
        LOG_F(INFO, "Stopping %s", source->name());
        source->stop();
    }

    // If another one is running, do nothing.
    for (auto s : m_sources) {
        if (s->isStarted()) {
            LOG_F(2, "Another source runnig: %s", s->name());
            return;
        }
    }

    // If another one wants to start, start it.
    for (auto s : m_sources) {
        if (s->wantsToStart()) {
            LOG_F(INFO, "Starting %s", s->name());
            s->start();
        }
    }
}
