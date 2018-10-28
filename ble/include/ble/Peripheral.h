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
    void peq(const QByteArray& value);
    void crossover(const QByteArray& value);
    void loudness(const QByteArray& value);

private:
    void startPublishing();
    void onCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);

    class PeripheralPrivate *const d = nullptr;
};

} // namespace ble
