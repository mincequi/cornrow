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

#include "Client.h"

#include "ClientSession.h"
#include "Converter.h"
#include "Defines.h"

#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtBluetooth/QLowEnergyAdvertisingParameters>

namespace ble
{

Client::Client(QObject *parent)
    : QObject(parent)
{
}

Client::~Client()
{
    delete m_clientSession;
}

bool Client::startDiscovering()
{
    if (m_clientSession) {
        delete m_clientSession;
    }

    // @TODO(mawe) this check always fails on iOS
    /*
    QBluetoothLocalDevice localDevice;
    if (!localDevice.isValid()) {
        setStatus(Status::NoBluetooth);
        return false;
    }*/

    setStatus(Status::Discovering, "Searching for cornrow devices");
    m_clientSession = new ClientSession(this);
    return true;
}

void Client::disconnect()
{
    delete m_clientSession;
    m_clientSession = nullptr;
}

void Client::writeCharacteristic(common::FilterGroup group, const QByteArray& value)
{
    if (!m_clientSession) {
        return;
    }

    const auto characteristic = m_clientSession->m_service->characteristic(m_clientSession->m_converter.toBle(group));
    if (!characteristic.isValid()) {
        qDebug() << __func__ << "Characteristic invalid:" << characteristic.uuid();
        return;
    }
    m_clientSession->m_service->writeCharacteristic(characteristic, value);
}

void Client::setStatus(Status _error, const QString& errorString)
{
    qDebug() << "Status:" << static_cast<int32_t>(_error) << "error:" << errorString;
    emit status(_error, errorString);
}

void Client::onCharacteristicRead(const QLowEnergyCharacteristic& characteristic, const QByteArray& value)
{
    qDebug() << __func__;
    emit status(Status::Connected);
    emit characteristicRead(m_clientSession->m_converter.fromBle(characteristic.uuid()), value);
}

} // namespace ble
