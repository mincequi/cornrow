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

#include <QDebug>
#include <QThread>
#include <QtDBus/QDBusObjectPath>
#include <Qt5GStreamer/QGst/Bin>
#include <Qt5GStreamer/QGst/ElementFactory>

namespace audio
{

Pipeline::Pipeline(const Configuration& configuration, QObject* parent)
    : QObject(parent)
{
    m_bluetoothSource = QGst::ElementFactory::make("avdtpsrc");
    auto depay = QGst::ElementFactory::make("rtpsbcdepay");
    auto parse = QGst::ElementFactory::make("sbcparse");
    auto decoder = QGst::ElementFactory::make("sbcdec");
    auto converter = QGst::ElementFactory::make("audioconvert");
    auto sink = QGst::ElementFactory::make("autoaudiosink");
    // Avoid resync since it causes ugly glitches.
    sink->setProperty("sync", false);

    m_pipeline = QGst::Pipeline::create();
    m_pipeline->add(m_bluetoothSource, depay, parse, decoder, converter, sink);
    bool success = QGst::Element::linkMany(m_bluetoothSource, depay, parse, decoder, converter, sink);

    m_bluetoothSource->setProperty("transport", configuration.transport);
    m_pipeline->setState(QGst::StatePlaying);
}

Pipeline::~Pipeline()
{
    m_pipeline->setState(QGst::StateNull);
}

} // namespace audio
