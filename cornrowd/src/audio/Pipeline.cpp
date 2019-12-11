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

#include <coro/audio/Loudness.h>
#include <coro/audio/Peq.h>
#include <coro/core/AppSource.h>
#include <coro/core/FdSource.h>
#include <Crossover.h>
#include <loguru/loguru.hpp>

#include "Converter.h"

#include <iostream>

namespace audio
{

GstBuffer * _buffer;
GstMapInfo  _mapInfo;

Pipeline::Pipeline(Type type)
{
    // Common elements
    m_crAppSource = Gst::ElementFactory::create_element("cr_appsrc");
    m_crFdSource = Gst::ElementFactory::create_element("cr_fdsrc");
    m_sbcDepay = Gst::ElementFactory::create_element("cr_rtpsbcdepay");
    m_sbcDecoder = Gst::ElementFactory::create_element("sbcdec");
    m_bluetoothConverter = Gst::AudioConvert::create();
    m_peq = Glib::RefPtr<coro::audio::Peq>::cast_static(Gst::ElementFactory::create_element("peq"));
    m_loudness = Glib::RefPtr<coro::audio::Loudness>::cast_static(Gst::ElementFactory::create_element("loudness"));

    // Normal elements
    m_alsaConverter = Gst::AudioConvert::create();
    m_alsaSink = Gst::AlsaSink::create("alsasink");
    m_alsaSink->set_property("sync", false);    // Avoid resync since it causes ugly glitches

    // Crossover elements
    m_crossover = Glib::RefPtr<coro::Crossover>::cast_static(Gst::ElementFactory::create_element("crossover"));
    m_ac3Encoder = Gst::ElementFactory::create_element("avenc_ac3");
    m_ac3Encoder->set_property("bitrate", 640000);
    m_alsaPassthroughSink = Gst::ElementFactory::create_element("alsapassthroughsink");
    m_alsaPassthroughSink->set_property("sync", false); // Avoid resync since it causes ugly glitches.
    m_alsaPassthroughSink->set_property("device", Glib::ustring("iec958:CARD=sndrpihifiberry,DEV=0"));

    constructPipeline(type);
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
    LOG_F(INFO, "Pipeline stoppped");
}

Pipeline::Type Pipeline::type() const
{
    return m_currentType;
}

void Pipeline::setVolume(float volume)
{
    m_loudness->setVolume(volume);
}

void Pipeline::setPeq(const std::vector<common::Filter>& filters)
{
    m_peq->setFilters(toCoro(filters));
}

void Pipeline::setCrossover(const common::Filter& crossover)
{
    m_crossover->setFrequency(crossover.f);

    if (crossover.f == 0.0) {
        constructPipeline(Type::Normal);
    } else {
        constructPipeline(Type::Crossover);
    }
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
    crossover.type = m_crossover->frequency() == 0.0f ? common::FilterType::Invalid : common::FilterType::CrossoverLr2;
    crossover.f = m_crossover->frequency();

    return crossover;
}

void Pipeline::pushBuffer(char* data, int maxSize, int size, int slices)
{
    auto buffer = gst_buffer_new_wrapped(data, maxSize);

    std::vector<GstBuffer*> subBuffers;
    for (int i = 1; i < slices; ++i) {
        subBuffers.push_back(gst_buffer_copy_region(buffer, GST_BUFFER_COPY_ALL, i*(size/slices), size/slices));
    }

    gst_buffer_resize(buffer, 0, size/slices);
    cr_app_source_push_buffer((CrAppSource*)m_crAppSource->gobj(), buffer);
    for (auto& subBuffer : subBuffers) {
        cr_app_source_push_buffer((CrAppSource*)m_crAppSource->gobj(), subBuffer);
    }

    start();
}

void Pipeline::setFileDescriptor(uint32_t sampleRate, int fd, uint32_t blockSize)
{
    CR_FD_SOURCE(m_crFdSource->gobj())->init(sampleRate, fd, blockSize, 7);
}

void Pipeline::constructPipeline(Type type)
{
    // No change in pipeline, do nothing
    if (type == m_currentType && m_pipeline) {
        return;
    }

    // Deconstruct existing one
    if (m_pipeline) {
        stop();
        m_pipeline.clear();
    }

    LOG_F(INFO, "Creating pipeline...");
    m_pipeline = Gst::Pipeline::create();

    switch (type) {
    case Type::Normal:
        m_pipeline->add(m_crAppSource)->add(m_sbcDepay)->add(m_sbcDecoder)->add(m_bluetoothConverter)->add(m_peq)->add(m_loudness)->add(m_alsaConverter)->add(m_alsaSink);
        m_crAppSource->link(m_sbcDepay)->link(m_sbcDecoder)->link(m_bluetoothConverter)->link(m_peq)->link(m_loudness)->link(m_alsaConverter)->link(m_alsaSink);
        break;
    case Type::Crossover:
        m_pipeline->add(m_crAppSource)->add(m_sbcDepay)->add(m_sbcDecoder)->add(m_bluetoothConverter)->add(m_peq)->add(m_loudness)->add(m_crossover)->add(m_ac3Encoder)->add(m_alsaPassthroughSink);
        m_crAppSource->link(m_sbcDepay)->link(m_sbcDecoder)->link(m_bluetoothConverter)->link(m_peq)->link(m_loudness)->link(m_crossover)->link(m_ac3Encoder)->link(m_alsaPassthroughSink);
        break;
    }

    m_currentType = type;
    LOG_F(INFO, "Pipeline created");
}

} // namespace audio
