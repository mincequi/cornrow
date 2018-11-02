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

#include <common/Types.h>

class QBluetoothUuid;
class QLowEnergyCharacteristic;

namespace ble
{

class Central : public QObject
{
    Q_OBJECT

public:
    enum class Error {
        None,
        NoService,
        InvalidCharacteristic
    };

    explicit Central(QObject *parent = nullptr);
    ~Central();

    bool startDiscovering();
    void disconnect();

    void writeCharacteristic(const QBluetoothUuid& uuid, const QByteArray &value);

signals:
    void error(Error error);

    // Emits initial values
    void characteristicRead(common::FilterTask task, const QByteArray &value);

private:
    void setError(Error error);
    void onCharacteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);

    class CentralPrivate *const d = nullptr;
    friend class CentralPrivate;
};

} // namespace ble
