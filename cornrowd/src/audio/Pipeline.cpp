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
#include <gstreamermm/pipeline.h>

#include <Peq.h>

#include "Converter.h"

namespace audio
{

Pipeline::Pipeline()
{
    m_bluetoothSource = Gst::ElementFactory::create_element("avdtpsrc");
    auto depay = Gst::ElementFactory::create_element("rtpsbcdepay");
    auto parse = Gst::ElementFactory::create_element("sbcparse");
    auto decoder = Gst::ElementFactory::create_element("sbcdec");
    auto conv1 = Gst::ElementFactory::create_element("audioconvert");
    m_peq = Glib::RefPtr<GstDsp::Peq>::cast_static(Gst::ElementFactory::create_element("peq"));
    auto conv2 = Gst::ElementFactory::create_element("audioconvert");
    auto sink = Gst::ElementFactory::create_element("autoaudiosink");
    // Avoid resync since it causes ugly glitches.
    sink->set_property("sync", false);

    m_pipeline = Gst::Pipeline::create();
    m_pipeline->add(m_bluetoothSource)->add(depay)->add(parse)->add(decoder)->add(conv1)->add(m_peq)->add(conv2)->add(sink);
    m_bluetoothSource->link(depay)->link(parse)->link(decoder)->link(conv1)->link(m_peq)->link(conv2)->link(sink);
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

} // namespace audio
