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

#include <QZeroProps/QZeroPropsClient.h>

class QBluetoothDeviceInfo;
class QLowEnergyCharacteristic;

namespace QZeroProps
{
class QZeroPropsService;

class QZeroPropsBluetoothLeService : public QObject
{
    Q_OBJECT

public:
    explicit QZeroPropsBluetoothLeService(QObject *parent = nullptr);
    ~QZeroPropsBluetoothLeService();

    void startDiscovering(const QUuid& uuid);

    void connectToService(QZeroProps::QZeroPropsService* service);
    void disconnect();

    void setCharacteristic(const std::string& uuid, const QByteArray& value);

signals:
    void status(QZeroPropsClient::State state, const QString& errorString = QString());
	void deviceDiscovered(const QBluetoothDeviceInfo& device);

    // Emits remotely changed values
    void characteristicChanged(const QUuid& uuid, const QByteArray& value);

private:
    void setStatus(QZeroPropsClient::State state, const QString& errorString = QString());

    class BleClientSession* m_clientSession = nullptr;
    friend class BleClientSession;
};

} // namespace QZeroProps
