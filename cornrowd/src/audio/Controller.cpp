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

#include "Controller.h"

#include <QDebug>
#include <QThread>
#include <QtDBus/QDBusObjectPath>
#include <Qt5GStreamer/QGst/Bin>
#include <Qt5GStreamer/QGst/ElementFactory>
#include <Qt5GStreamer/QGst/Init>

#include <gstreamermm-dsp.h>
#include <common/Types.h>

#include "Pipeline.h"

namespace audio
{

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    // Init QtGStreamer
    QGst::init();
    // Init gstreamermm-dsp
    GstDsp::init();
}

const std::vector<common::Filter> Controller::peq() const
{
    // @TODO: get real values
    return {
        { common::FilterType::Peak, 2335.0, -9.7, 15.7 },
        { common::FilterType::LowPass, 12222.0, 3.4, 0.5 },
        { common::FilterType::HighPass, 47.0, -8.3, 0.8 }
    };
}

void Controller::setPeq(const std::vector<common::Filter>& filters)
{
    // @TODO: set values to pipeline
    for (const auto& filter : filters) {
        qDebug() << "type:" << static_cast<uint>(filter.type) << ", f:" << filter.f << ", g:" << filter.g << ", q:" << filter.q;
    }
    qDebug() << "";
}

void Controller::setTransport(const QDBusObjectPath& transport)
{
    if (m_pipeline) {
        qDebug() << "Another device is connected";
        return;
    }

    m_pipeline = new Pipeline({transport.path()});
    QMetaObject::invokeMethod(m_pipeline, &audio::Pipeline::start, Qt::QueuedConnection);
}

void Controller::clearTransport()
{
    m_pipeline->stop();
    //QMetaObject::invokeMethod(m_pipeline, &audio::Pipeline::stop, Qt::QueuedConnection);

    if (m_pipeline) {
        delete m_pipeline;
        m_pipeline = nullptr;
    }
}

} // namespace audio
