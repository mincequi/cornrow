/*
 * Copyright (C) 2020 Manuel Weichselbaumer <mincequi@web.de>
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

#pragma once

#include "QZeroPropsServicePrivate.h"
#include "QZeroPropsBleClient.h"
#include "BleClientSession.h"
#include <QtBluetooth/QBluetoothSocket>

namespace QtZeroProps
{

class QZeroPropsBleService : public QZeroPropsServicePrivate
{
public:
    QZeroPropsBleService(QZeroPropsService* _q)
        : QZeroPropsServicePrivate(_q),
          session(new BleClientSession(this))
    {
        QObject::connect(session, &BleClientSession::characteristicChanged, [this](const QUuid& uuid, const QByteArray& value) {
            onReceive(uuid, value);
        });
    }

    ~QZeroPropsBleService() override {
        delete session;
    }

    void connect() override {
        emit stateChanged(QZeroPropsClient::State::Connecting, "Connecting " + q->name());
        session->connectToDevice(bluetoothDeviceInfo, serviceUuid);
    }

    void disconnect() override {
        session->disconnectFromDevice();
    }

    void doSend(const QVariant& uuid, const QByteArray& value) override {
        if (!session || !session->m_service) {
            qWarning() << __func__ << "> No BLE device connected";
            return;
        }

        const auto characteristic = session->m_service->characteristic(uuid.toUuid());
        if (!characteristic.isValid()) {
            qDebug() << __func__ << "> Characteristic invalid:" << characteristic.uuid();
            return;
        }
        session->m_service->writeCharacteristic(characteristic, value);
    }

    void onReceive(const QUuid& uuid, const QByteArray& value) {
        properties.insert(uuid, value);
        emit q->propertyChanged(uuid, value);
    }

    // Event handlers from socket
    void onStateChanged(QAbstractSocket::SocketState state);

    QBluetoothDeviceInfo    bluetoothDeviceInfo;
    QUuid                   serviceUuid;
    BleClientSession*       session = nullptr;
};

} // namespace QZeroProps
