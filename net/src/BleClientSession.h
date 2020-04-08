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

#include <QObject>

#include <QtBluetooth/QLowEnergyAdvertisingParameters>

namespace QtZeroProps
{
class QZeroPropsServicePrivate;

class BleClientSession : public QObject
{
    Q_OBJECT

public:
    BleClientSession(QZeroPropsServicePrivate* _q);
    ~BleClientSession();

    void setDiscoveryTimeout(int msTimeout);
    void startDiscovery(const QUuid& uuid);
    void stopDiscovery();

    void connectToDevice(const QBluetoothDeviceInfo& device, const QUuid& serviceUuid);
    void disconnectFromDevice();

signals:
    // Emits remotely changed values
    void characteristicChanged(const QUuid& uuid, const QByteArray& value);

private:
    // QLowEnergyController
    void onDeviceConnected();
    void onDeviceDisconnected();
    void onServiceDiscovered(const QBluetoothUuid&);
    void onServiceDiscoveryError(QLowEnergyController::Error);
    void onServiceDiscoveryFinished();

    // QLowEnergyService
    void onServiceStateChanged(QLowEnergyService::ServiceState s);
    void onServiceError(QLowEnergyService::ServiceError error);
    void onCharacteristicRead(const QLowEnergyCharacteristic& characteristic, const QByteArray& value);

    QZeroPropsServicePrivate* q;
    QUuid                   m_serviceUuid;
    QLowEnergyController*   m_control = nullptr;
    QLowEnergyService*      m_service = nullptr;

    friend class QZeroPropsBleService;
};

} // namespace QZeroProps
