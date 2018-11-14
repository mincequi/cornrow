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

#include "Server.h"

#include "Defines.h"
#include "ServerSession.h"

#include <QtBluetooth/QLowEnergyAdvertisingParameters>
#include <QtBluetooth/QLowEnergyCharacteristicData>
#include <QtBluetooth/QLowEnergyController>
#include <QtBluetooth/QLowEnergyServiceData>

namespace ble
{

Server::Server(QObject *parent)
    : QObject(parent)
{
}

Server::~Server()
{
}

void Server::init(CharcsProvider charcsProvider)
{
    m_charcsProvider = charcsProvider;

    startPublishing();
}

void Server::startPublishing()
{
    if (m_session) {
        delete m_session;
        m_session = nullptr;
    }

    // Publish service
    m_session = new ServerSession(this, m_charcsProvider());
    // Advertising will stop once a client connects, so re-advertise once disconnected.
    connect(m_session->peripheral, &QLowEnergyController::disconnected, this, &Server::startPublishing);
    connect(m_session->peripheral, &QLowEnergyController::disconnected, this, &Server::deviceDisconnected);
    connect(m_session->peripheral, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error), this, &Server::deviceDisconnected);
}

} // namespace ble
