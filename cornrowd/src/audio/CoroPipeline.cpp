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
#include <coro/pi/PiHdmiAudioSink.h>
#include <loguru/loguru.hpp>

#include "Converter.h"

using namespace std::placeholders;
using namespace coro::core;

CoroPipeline::CoroPipeline() :
    m_airplaySource( { "myAirplay" } ),
    m_ac3Encoder(coro::audio::AudioCodec::Ac3)
{
    m_loudness = new coro::audio::Loudness();
    m_peq = new coro::audio::Peq();

#ifdef TARGET_PI
    m_piHdmiSink = new coro::pi::PiHdmiAudioSink();
#endif

    //m_alsaSink.setDevice("iec958:CARD=sndrpihifiberry,DEV=0");

    // Airplay nodes
    Node::link(m_airplaySource, m_intToFloat);

    // Bluetooth nodes
    Node::link(m_fdSource, m_rtpDecoder);
    Node::link(m_rtpDecoder, m_sbcDecoder);
    Node::link(m_sbcDecoder, m_intToFloat);

    // Scream nodes
    //Node::link(m_screamSource, m_intToFloat);

    Node::link(m_intToFloat, *m_peq);
    Node::link(*m_peq, *m_loudness);
    Node::link(*m_loudness, m_crossover);

    if (m_piHdmiSink) {
        Node::link(m_crossover, m_floatToInt);
        Node::link(m_floatToInt, *m_piHdmiSink);
    } else {
        Node::link(m_crossover, m_ac3Encoder);
        Node::link(m_ac3Encoder, m_floatToInt);
        Node::link(m_floatToInt, m_alsaSink);
    }

    m_sourceSelector.addSource(m_airplaySource);
    m_sourceSelector.addSource(m_fdSource);
}

CoroPipeline::~CoroPipeline()
{
    delete m_peq;
    delete m_loudness;
}

void CoroPipeline::setFileDescriptor(int fd, uint16_t blockSize)
{
    m_fdSource.init(fd, blockSize);
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
    m_floatToInt.setIsBypassed(crossover.isValid() && !m_piHdmiSink);
}

void CoroPipeline::setOutputDevice(const std::string& device)
{
    m_alsaSink.setDevice(device);
}

bool CoroPipeline::hasPiHdmiOutput() const
{
    return m_piHdmiSink != nullptr;
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
