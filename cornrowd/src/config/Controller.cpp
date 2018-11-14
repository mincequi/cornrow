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

namespace config
{

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    // On start-up we read config from disk
    m_peq = readConfig();

    // Create BLE server and adapter. Provide config provider.
    m_ble = new ble::Server(this);
    m_bleAdapter = new ble::ServerAdapter(m_ble, readConfig);

    // BLE adapter can change config
    connect(m_bleAdapter, &ble::ServerAdapter::peq, this, &Controller::onPeqChanged);
}

void Controller::setAudioController(audio::Controller* audio)
{
    // Assign new audio controller (and apply config).
    m_audio = audio;
    m_audio->setPeq(m_peq);
}

void Controller::unsetAudioController()
{
    // If connection is closed, we write config to disk
    writeConfig(m_audio->peq());
    m_audio = nullptr;
}

void Controller::onPeqChanged(const std::vector<common::Filter>& filters)
{
    // Assign new config (from BLE) to local object
    m_peq = filters;

    // Also set config to audio controller (if existing).
    if (m_audio) {
        m_audio->setPeq(m_peq);
    }
}

} // namespace config
