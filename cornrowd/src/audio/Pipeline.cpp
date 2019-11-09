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

#include <coro/core/AppSource.h>
#include <coro/core/FdSource.h>
#include <Crossover.h>
#include <Loudness.h>
#include <Peq.h>

#include "Converter.h"

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
    m_crFdSource = Gst::ElementFactory::create_element("cr_fdsrc");
    auto depay = Gst::ElementFactory::create_element("cr_rtpsbcdepay");
    auto parse = Gst::ElementFactory::create_element("cr_sbcparse");
    auto decoder = Gst::ElementFactory::create_element("sbcdec");
    auto bluetoothConverter = Gst::AudioConvert::create();
    m_peq = Glib::RefPtr<GstDsp::Peq>::cast_static(Gst::ElementFactory::create_element("peq"));
    m_loudness = Glib::RefPtr<GstDsp::Loudness>::cast_static(Gst::ElementFactory::create_element("loudness"));
    auto alsaConverter = Gst::AudioConvert::create();
    m_alsaSink = Gst::AlsaSink::create("alsasink");
    m_alsaSink->set_property("sync", false);    // Avoid resync since it causes ugly glitches

    m_pipeline->add(m_crFdSource)->add(depay)/*->add(parse)*/->add(decoder)->add(bluetoothConverter)->add(m_peq)->add(m_loudness)->add(alsaConverter)->add(m_alsaSink);
    m_crFdSource->link(depay)/*->link(parse)*/->link(decoder)->link(bluetoothConverter)->link(m_peq)->link(m_loudness)->link(alsaConverter)->link(m_alsaSink);

    // Crossover output
    m_crossover = Glib::RefPtr<GstDsp::Crossover>::cast_static(Gst::ElementFactory::create_element("crossover"));
    //auto ac3Encoder = Gst::ElementFactory::create_element("avenc_ac3");
    //ac3Encoder->set_property("bitrate", 640000);
    m_alsaPassthroughSink = Gst::ElementFactory::create_element("alsapassthroughsink");
    m_alsaPassthroughSink->set_property("sync", false); // Avoid resync since it causes ugly glitches.
    m_alsaPassthroughSink->set_property("device", Glib::ustring("iec958:CARD=sndrpihifiberry,DEV=0"));
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

void Pipeline::setPeq(const std::vector<common::Filter>& filters)
{
    m_peq->setFilters(toGstDsp(filters));
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
}

void Pipeline::setFileDescriptor(int fd, uint32_t blockSize)
{
    CR_FD_SOURCE(m_crFdSource->gobj())->init(fd, blockSize, 7);
}

} // namespace audio
