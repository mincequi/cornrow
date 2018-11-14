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

Controller::Controller(audio::Controller* audio, QObject *parent)
    : QObject(parent),
      m_audio(audio)
{
    // On start-up we read config from disk
    m_audio->setPeq(readConfig());

    // Create BLE server and adapter. Provide config provider.
    m_ble = new ble::Server(this);
    m_bleAdapter = new ble::ServerAdapter(m_ble, std::bind(&audio::Controller::peq, m_audio));

    // BLE adapter can change config of audio controller
    connect(m_bleAdapter, &ble::ServerAdapter::peq, m_audio, &audio::Controller::setPeq);
}

void Controller::writeConfig()
{
    // If connection is closed, we write config to disk
    config::writeConfig(m_audio->peq());
}

} // namespace config
