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

#pragma once

#include <QObject>

#include <common/ble/Types.h>

class QBluetoothDeviceInfo;
class QLowEnergyCharacteristic;

namespace ble
{

class BleClient : public QObject
{
    Q_OBJECT

public:
    enum class Status {
        NoBluetooth,    // Currently unused, since we cannot tell on iOS wether Bluetooth is on.
        Discovering,
        Connecting,     // Connecting state hangs from time to time, so provide user interaction
        Connected,
        Timeout,
        Lost,
        Error
    };

    explicit BleClient(QObject *parent = nullptr);
    ~BleClient();

    void startDiscovering(const QUuid& uuid);

    void connectDevice(const QBluetoothDeviceInfo& device);
    void disconnect();

    void setCharacteristic(const std::string& uuid, const QByteArray& value);

signals:
    void status(Status status, const QString& errorString = QString());
	void deviceDiscovered(const QBluetoothDeviceInfo& device);

    // Emits remotely changed values
    void characteristicChanged(const QUuid& uuid, const QByteArray& value);

private:
    void setStatus(Status status, const QString& errorString = QString());

    class ClientSession* m_clientSession = nullptr;
    friend class ClientSession;
};

} // namespace ble
