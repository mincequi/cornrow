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

#include <functional>

#include <QObject>
#include <QtBluetooth/QBluetoothUuid>

class QLowEnergyCharacteristic;

namespace QtZeroProps
{

class BleServer : public QObject
{
    Q_OBJECT

public:
    explicit BleServer(QObject *parent = nullptr);
    ~BleServer();

signals:
    // Signals, that a client has changed a characteristic
    void characteristicChanged(const QLowEnergyCharacteristic& characteristic, const QByteArray& newValue);
    // Signals, that a device disconnected. Used to trigger persistence.
    void deviceDisconnected();

private:
    // Adapter should set initial values of characteristics
    using CharcsProvider = std::function<std::map<QBluetoothUuid, QByteArray>()>;
    void init(const QUuid& uuid, CharcsProvider);

    void startPublishing(const QUuid& uuid);

    CharcsProvider m_charcsProvider = nullptr;

    class BleServerSession* m_session = nullptr;
    friend class BleServerAdapter;
    friend class BleServerSession;
};

} // namespace QZeroProps
