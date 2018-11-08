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
    qDebug() << __func__ << configuration.transport;

    m_bluetoothSource = QGst::ElementFactory::make("avdtpsrc");
    auto depay = QGst::ElementFactory::make("rtpsbcdepay");
    auto parse = QGst::ElementFactory::make("sbcparse");
    auto decoder = QGst::ElementFactory::make("sbcdec");
    auto conv1 = QGst::ElementFactory::make("audioconvert");
    m_peq = QGst::ElementFactory::make("peq");
    auto conv2 = QGst::ElementFactory::make("audioconvert");
    auto sink = QGst::ElementFactory::make("autoaudiosink");
    // Avoid resync since it causes ugly glitches.
    sink->setProperty("sync", false);

    m_pipeline = QGst::Pipeline::create();
    m_pipeline->add(m_bluetoothSource, depay, parse, decoder, conv1, m_peq, conv2, sink);
    bool success = QGst::Element::linkMany(m_bluetoothSource, depay, parse, decoder, conv1, m_peq, conv2, sink);

    m_bluetoothSource->setProperty("transport", configuration.transport);
}

Pipeline::~Pipeline()
{
    stop();
}

void Pipeline::start()
{
    m_pipeline->setState(QGst::StatePlaying);
}

void Pipeline::stop()
{
    m_pipeline->setState(QGst::StateNull);
}

/*
GstDsp::Peq* Pipeline::peq()
{
    return nullptr; //m_peq->object<GstDsp::Peq*>();
}
*/

} // namespace audio
