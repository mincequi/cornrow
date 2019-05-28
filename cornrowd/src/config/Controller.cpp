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

#include "Persistence.h"

using namespace std::placeholders;

namespace config
{

Controller::Controller(audio::Controller* audio,
                       bluetooth::Controller* bluetooth,
                       QObject* parent)
    : QObject(parent),
      m_audio(audio),
      m_bluetooth(bluetooth)
{
    // On start-up we read config from disk
    m_audio->setFilters(common::FilterGroup::Peq, readPeq());
    //m_audio->setFilters(common::FilterGroup::Aux, readPeq());

    // Create BLE server and adapter. Provide config provider.
    //m_ble = new ble::Server(this);
    //m_bleAdapter = new ble::ServerAdapter(m_ble, std::bind(&audio::Controller::peq, m_audio));

    m_bluetooth->setReadFiltersCallback(std::bind(&audio::Controller::filters, m_audio, _1));
    connect(m_bluetooth, &bluetooth::Controller::filtersWritten, m_audio, &audio::Controller::setFilters);

    // Once a (control) client disconnects, we write persistence.
    //connect(m_ble, &ble::Server::deviceDisconnected, this, &config::Controller::writeConfig);

    // BLE adapter can change config of audio controller
    //connect(m_bleAdapter, &ble::ServerAdapter::peq, m_audio, &audio::Controller::setPeq);
}

Controller::~Controller()
{
}

void Controller::writeConfig()
{
    // If connection is closed, we write config to disk
    config::writeConfig(m_audio->filters(common::FilterGroup::Peq));
}

} // namespace config
