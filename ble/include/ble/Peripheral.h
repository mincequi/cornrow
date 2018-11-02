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

class QBluetoothUuid;
class QLowEnergyCharacteristic;

namespace ble
{

class Peripheral : public QObject
{
    Q_OBJECT

public:
    explicit Peripheral(QObject *parent = nullptr);
    ~Peripheral();

signals:
    // Signals, that a client has changed a characteristic
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);

private:
    // Adapter should set initial values of characteristics
    void setCharacteristics(const std::map<QBluetoothUuid, QByteArray>& characteristics);

    void startPublishing();

    class PeripheralPrivate *const d = nullptr;
    friend class PeripheralAdapter;
};

} // namespace ble
