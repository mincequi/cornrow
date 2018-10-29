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

#include <common/Types.h>

#include "Pipeline.h"

namespace audio
{

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    // Init GStreamer
    QGst::init();
}

const std::vector<common::Filter> Controller::peq() const
{
    return {
        { common::FilterType::Peak, 8755.0, -9.7, 0.7 },
        { common::FilterType::LowPass, 513.0, 3.4, 0.5 },
        { common::FilterType::HighPass, 4007.0, -8.3, 0.11 }
    };
}

void Controller::setPeq(const std::vector<common::Filter>& filters)
{

}

void Controller::setTransport(const QDBusObjectPath& transport)
{
    if (m_pipeline) {
        qDebug() << "Another device is connected";
        return;
    }
    m_pipeline = new Pipeline({transport.path()}, this);
}

void Controller::clearTransport()
{
    if (m_pipeline) {
        delete m_pipeline;
        m_pipeline = nullptr;
    }
}

} // namespace audio
