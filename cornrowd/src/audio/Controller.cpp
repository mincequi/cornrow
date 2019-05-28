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

std::vector<common::Filter> Controller::filters(common::FilterGroup group) const
{
    switch (group) {
    case common::FilterGroup::Peq:
        return m_pipeline->peq();
        break;
    case common::FilterGroup::Aux:
        return { m_pipeline->crossover() };
        break;
    case common::FilterGroup::Invalid:
        return {};
    }
    return {};
}

void Controller::setFilters(common::FilterGroup group, const std::vector<common::Filter>& filters)
{
    for (const auto& filter : filters) {
        qDebug() << "group:" << static_cast<uint>(group) <<
                    "type:" << static_cast<uint>(filter.type) <<
                    ", f:" << filter.f <<
                    ", g:" << filter.g <<
                    ", q:" << filter.q;
    }
    qDebug() << "";

    switch (group) {
    case common::FilterGroup::Peq:
        m_pipeline->setPeq(filters);
        break;
    case common::FilterGroup::Aux: {
        // Check if crossover was provided, if not, we disable crossover
        auto it = std::find_if(filters.begin(), filters.end(), [](const common::Filter& f) {
            return f.type == common::FilterType::Crossover;
        });
        it != filters.end() ? m_pipeline->setCrossover(*it) : m_pipeline->setCrossover(common::Filter());
        break;
    }
    case common::FilterGroup::Invalid:
        break;
    }
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
