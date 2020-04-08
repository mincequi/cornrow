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

#include <QtZeroProps/QZeroPropsClient.h>

#include <QBluetoothDeviceDiscoveryAgent>

class QBluetoothDeviceInfo;
class QLowEnergyController;

namespace QtZeroProps
{
class QZeroPropsService;

class QZeroPropsBleClient : public QObject
{
    Q_OBJECT

public:
    explicit QZeroPropsBleClient(QObject *parent = nullptr);
    ~QZeroPropsBleClient();

    void setDiscoveryTimeout(int msTimeout);
    void startDiscovery(const QUuid& uuid);
    void stopDiscovery();

signals:
    void stateChanged(QZeroPropsClient::State state, const QString& errorString = QString());
    void serviceDiscovered(const QBluetoothDeviceInfo& device, const QUuid& serviceUuid);

private:
    void setStatus(QZeroPropsClient::State state, const QString& errorString = QString());

    void onDeviceDiscovered(const QBluetoothDeviceInfo& device);
    void onDeviceDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error error);
    void onDeviceDiscoveryFinished();

    void discoverServices(const QBluetoothDeviceInfo& device);

    QUuid   m_serviceUuid;
    QBluetoothDeviceDiscoveryAgent* m_discoverer = nullptr;
    QLowEnergyController* m_controller;

    friend class BleClientSession;
    friend class QZeroPropsBleService;
    friend class QZeroPropsClientPrivate;

    int m_msTimeout = 8000;
};

} // namespace QZeroProps
