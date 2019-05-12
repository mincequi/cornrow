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

#include <gstreamermm/element.h>
#include <gstreamermm/outputselector.h>
#include <gstreamermm/pipeline.h>

#include <Crossover.h>
#include <Loudness.h>
#include <Peq.h>

#include "Converter.h"

namespace audio
{

Pipeline::Pipeline()
{
    m_pipeline = Gst::Pipeline::create();

    // Bluetooth related elements
    m_bluetoothSource = Gst::ElementFactory::create_element("avdtpsrc");
    auto depay = Gst::ElementFactory::create_element("rtpsbcdepay");
    auto parse = Gst::ElementFactory::create_element("sbcparse");
    auto decoder = Gst::ElementFactory::create_element("sbcdec");
    auto bluetoothConverter = Gst::ElementFactory::create_element("audioconvert");
    m_pipeline->add(m_bluetoothSource)->add(depay)->add(parse)->add(decoder)->add(bluetoothConverter);
    m_bluetoothSource->link(depay)->link(parse)->link(decoder)->link(bluetoothConverter);

    // Peq
    m_peq = Glib::RefPtr<GstDsp::Peq>::cast_static(Gst::ElementFactory::create_element("peq"));
    m_pipeline->add(m_peq);

    // Regular output
    auto alsaConverter = Gst::ElementFactory::create_element("audioconvert");
    m_alsaSink = Gst::ElementFactory::create_element("alsasink");
    m_alsaSink->set_property("sync", false);    // Avoid resync since it causes ugly glitches.
    m_pipeline->add(alsaConverter)->add(m_alsaSink);
    alsaConverter->link(m_alsaSink);

    // Crossover related elements
    m_crossover = Glib::RefPtr<GstDsp::Crossover>::cast_static(Gst::ElementFactory::create_element("crossover"));
    auto ac3Encoder = Gst::ElementFactory::create_element("avenc_ac3");
    ac3Encoder->set_property("bitrate", 640000);
    /*
    m_alsaPassthroughSink = Gst::ElementFactory::create_element("alsapassthroughsink");
    m_alsaPassthroughSink->set_property("sync", false); // Avoid resync since it causes ugly glitches.
    m_alsaPassthroughSink->set_property("device", Glib::ustring("iec958:CARD=sndrpihifiberry,DEV=0"));
    m_pipeline->add(m_crossover)->add(ac3Encoder)->add(m_alsaPassthroughSink);
    //m_crossover->link(ac3Encoder)->link(m_alsaPassthroughSink);
    */

    // Output selector
    m_outputSelector = Gst::OutputSelector::create();
    m_outputSelector->property_pad_negotiation_mode().set_value(Gst::OUTPUT_SELECTOR_PAD_NEGOTIATION_MODE_ACTIVE);
    m_regularPad = m_outputSelector->get_request_pad("src_%u");
    m_passthroughPad = m_outputSelector->get_request_pad("src_%u");
    m_pipeline->add(m_outputSelector);

    // Bluetooth -> Peq -> OutoutSelector -> Sink
    //                                    \-> Crossover -> PassthroughSink
    bluetoothConverter->link(m_peq)->link(m_outputSelector);
    m_regularPad->link(alsaConverter->get_static_pad("sink"));
    m_passthroughPad->link(m_crossover->get_static_pad("sink"));

    // Select regular pad
    m_outputSelector->property_active_pad().set_value(m_regularPad);
}

Pipeline::~Pipeline()
{
    stop();
}

void Pipeline::start(const std::string& transport)
{
    m_bluetoothSource->set_property("transport", transport);
    m_pipeline->set_state(Gst::STATE_PLAYING);
}

void Pipeline::stop()
{
    m_pipeline->set_state(Gst::STATE_NULL);
}

void Pipeline::setPeq(const std::vector<common::Filter> filters)
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

} // namespace audio
