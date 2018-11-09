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

#include <gstreamermm-dsp.h>
#include <common/Types.h>

#include "Pipeline.h"

namespace audio
{

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    // Init gstreamermm-dsp
    GstDsp::init();

    m_pipeline = new Pipeline();
}

Controller::~Controller()
{
    delete m_pipeline;
}

const std::vector<common::Filter> Controller::peq() const
{
    return m_pipeline->peq();
}

void Controller::setPeq(const std::vector<common::Filter>& filters)
{
    for (const auto& filter : filters) {
        qDebug() << "type:" << static_cast<uint>(filter.type) << ", f:" << filter.f << ", g:" << filter.g << ", q:" << filter.q;
    }
    qDebug() << "";

    m_pipeline->setPeq(filters);
}

void Controller::setTransport(const QDBusObjectPath& transport)
{
    m_pipeline->start({transport.path().toStdString()});
}

void Controller::clearTransport()
{
    m_pipeline->stop();
}

} // namespace audio
