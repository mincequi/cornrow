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

#include <gstreamermm/outputselector.h>
#include <gstreamermm/pipeline.h>

#include <Crossover.h>
#include <Loudness.h>
#include <Peq.h>

#include "Converter.h"

namespace audio
{

Pipeline::Pipeline(Type type)
{
    m_pipeline = Gst::Pipeline::create();

    // Common elements
    m_bluetoothSource = Gst::ElementFactory::create_element("avdtpsrc");
    auto depay = Gst::ElementFactory::create_element("rtpsbcdepay");
    auto parse = Gst::ElementFactory::create_element("sbcparse");
    auto decoder = Gst::ElementFactory::create_element("sbcdec");
    auto bluetoothConverter = Gst::AudioConvert::create();
    m_peq = Glib::RefPtr<GstDsp::Peq>::cast_static(Gst::ElementFactory::create_element("peq"));
    m_pipeline->add(m_bluetoothSource)->add(depay)->add(parse)->add(decoder)->add(bluetoothConverter)->add(m_peq);
    m_bluetoothSource->link(depay)->link(parse)->link(decoder)->link(bluetoothConverter)->link(m_peq);

    // Normal output
    auto alsaConverter = Gst::AudioConvert::create();
    m_alsaSink = Gst::AlsaSink::create("alsasink");
    m_alsaSink->set_property("sync", false);    // Avoid resync since it causes ugly glitches

    // Crossover output
    m_crossover = Glib::RefPtr<GstDsp::Crossover>::cast_static(Gst::ElementFactory::create_element("crossover"));
    auto ac3Encoder = Gst::ElementFactory::create_element("avenc_ac3");
    ac3Encoder->set_property("bitrate", 640000);
    m_alsaPassthroughSink = Gst::ElementFactory::create_element("alsapassthroughsink");
    m_alsaPassthroughSink->set_property("sync", false); // Avoid resync since it causes ugly glitches.
    m_alsaPassthroughSink->set_property("device", Glib::ustring("iec958:CARD=sndrpihifiberry,DEV=0"));
    //m_pipeline->add(m_crossover)->add(ac3Encoder)->add(m_alsaPassthroughSink);

    m_elements[Type::Normal]    = { alsaConverter, m_alsaSink };
    m_elements[Type::Crossover] = { m_crossover, ac3Encoder, m_alsaPassthroughSink };

    constructPipeline(type, true);
}

Pipeline::~Pipeline()
{
    m_pipeline->set_state(Gst::STATE_NULL);
}

Pipeline::Type Pipeline::type() const
{
    return m_currentType;
}

void Pipeline::setTransport(const std::string& transport)
{
    if (transport.empty()) {
        m_pipeline->set_state(Gst::STATE_NULL);
    } else {
        m_bluetoothSource->set_property("transport", transport);
        m_pipeline->set_state(Gst::STATE_PLAYING);
    }
}

void Pipeline::setPeq(const std::vector<common::Filter>& filters)
{
    m_peq->setFilters(toGstDsp(filters));
}

std::vector<common::Filter> Pipeline::peq() const
{
    return fromGstDsp(m_peq->filters());
}

void Pipeline::setCrossover(const common::Filter& crossover)
{
    m_crossover->setFrequency(crossover.f);
}

common::Filter Pipeline::crossover() const
{
    common::Filter crossover;
    crossover.type = m_crossover->frequency() == 0.0f ? common::FilterType::Invalid : common::FilterType::Crossover;
    crossover.f = m_crossover->frequency();

    return crossover;
}

bool Pipeline::constructPipeline(Type type, bool force)
{
    if (m_currentType == type && !force) {
        return true;
    }

    // Remove all elements which are not of current type
    for (auto kv : m_elements) {
        if (kv.first == type) {
            continue;
        }
        for (auto element : kv.second) {
            if (element->has_as_ancestor(m_pipeline)) {
                m_pipeline->remove(element);
            }
        }
    }

    // Add elements which are of current type
    for (auto it = m_elements.at(type).begin(); it != m_elements.at(type).end(); ++it) {
        m_pipeline->add(*it);
        if (it == m_elements.at(type).begin()) {
            m_peq->link(*it);
        } else {
            (*(std::prev(it)))->link(*it);
        }
    }

    m_currentType = type;

    return true;
}

} // namespace audio
