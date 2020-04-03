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

#include "BleServer.h"

#include "Defines.h"
#include "BleServerSession.h"

#include <QtBluetooth/QLowEnergyAdvertisingParameters>
#include <QtBluetooth/QLowEnergyCharacteristicData>
#include <QtBluetooth/QLowEnergyController>
#include <QtBluetooth/QLowEnergyServiceData>

namespace QZeroProps
{

BleServer::BleServer(QObject *parent)
    : QObject(parent)
{
}

BleServer::~BleServer()
{
}

void BleServer::init(CharcsProvider charcsProvider)
{
    m_charcsProvider = charcsProvider;

    startPublishing();
}

void BleServer::startPublishing()
{
    if (m_session) {
        delete m_session;
        m_session = nullptr;
    }

    // Publish service
    m_session = new BleServerSession(this, m_charcsProvider());
}

} // namespace QZeroProps
