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

#include "Pipeline.h"

#include <AppSource.h>
#include <Crossover.h>
#include <Loudness.h>
#include <Peq.h>

#include <iostream>

namespace audio
{

GstBuffer * _buffer;
GstMapInfo  _mapInfo;

Pipeline::Pipeline(Type type)
{
    m_pipeline = Gst::Pipeline::create();

    // Common elements
    m_crAppSource = Gst::ElementFactory::create_element("cr_appsrc");
    auto queue = Gst::Queue::create();
    auto depay = Gst::ElementFactory::create_element("rtpsbcdepay");
    auto parse = Gst::ElementFactory::create_element("sbcparse");
    auto decoder = Gst::ElementFactory::create_element("sbcdec");
    auto bluetoothConverter = Gst::AudioConvert::create();
    m_peq = Glib::RefPtr<GstDsp::Peq>::cast_static(Gst::ElementFactory::create_element("peq"));
    m_loudness = Glib::RefPtr<GstDsp::Loudness>::cast_static(Gst::ElementFactory::create_element("loudness"));
    auto alsaConverter = Gst::AudioConvert::create();
    m_alsaSink = Gst::AlsaSink::create("alsasink");
    m_alsaSink->set_property("sync", false);    // Avoid resync since it causes ugly glitches

    m_pipeline->add(m_crAppSource)->add(queue)->add(depay)->add(parse)->add(decoder)->add(bluetoothConverter)->add(m_peq)->add(m_loudness)->add(alsaConverter)->add(m_alsaSink);
    m_crAppSource->link(queue)->link(depay)->link(parse)->link(decoder)->link(bluetoothConverter)->link(m_peq)->link(m_loudness)->link(alsaConverter)->link(m_alsaSink);
}

Pipeline::~Pipeline()
{
    m_pipeline->set_state(Gst::STATE_NULL);
}

void Pipeline::start()
{
    m_pipeline->set_state(Gst::STATE_PLAYING);
}

void Pipeline::stop()
{
    m_pipeline->set_state(Gst::STATE_NULL);
}

Pipeline::Type Pipeline::type() const
{
    return m_currentType;
}

void Pipeline::setVolume(float volume)
{
    m_loudness->setVolume(volume);
}

void Pipeline::setCrossover(const common::Filter& crossover)
{
    m_crossover->setFrequency(crossover.f);
}

void Pipeline::setLoudness(uint8_t phon)
{
    m_loudness->setLevel(phon);
}

void Pipeline::setOutputDevice(const std::string& device)
{
    m_alsaPassthroughSink->set_property("device", Glib::ustring(device));
    m_alsaSink->set_property("device", Glib::ustring(device));
}

common::Filter Pipeline::crossover() const
{
    common::Filter crossover;
    crossover.type = m_crossover->frequency() == 0.0f ? common::FilterType::Invalid : common::FilterType::Crossover;
    crossover.f = m_crossover->frequency();

    return crossover;
}

void* Pipeline::obtainBuffer(int maxSize)
{
    _buffer = gst_buffer_new_and_alloc(maxSize);
    gst_buffer_map(_buffer, &_mapInfo, GST_MAP_WRITE);
    return _mapInfo.data;
}

void Pipeline::commitBuffer(int size)
{
    gst_buffer_unmap(_buffer, &_mapInfo);
    gst_buffer_resize(_buffer, 0, size);
    cr_app_source_push_buffer((CrAppSource*)m_crAppSource->gobj(), _buffer);
}

} // namespace audio
