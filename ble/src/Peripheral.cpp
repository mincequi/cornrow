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

#include "Peripheral.h"

#include "Defines.h"

#include <QtBluetooth/QLowEnergyAdvertisingParameters>
#include <QtBluetooth/QLowEnergyCharacteristicData>
#include <QtBluetooth/QLowEnergyController>
#include <QtBluetooth/QLowEnergyServiceData>

namespace ble
{

struct PeripheralPrivate
{
    PeripheralPrivate(Peripheral* q)
    {
        peripheral = QLowEnergyController::createPeripheral(q);

        advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
        advertisingData.setServices({cornrowServiceUuid});

        // Peq characteristic
        QLowEnergyCharacteristicData charData;
        charData.setUuid(peqCharacteristicUuid);
        charData.setValue(QByteArray("abc123"));
        charData.setProperties(QLowEnergyCharacteristic::Read | QLowEnergyCharacteristic::Write);

        // Service
        serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
        serviceData.setUuid(cornrowServiceUuid);
        serviceData.addCharacteristic(charData);
    }

    QLowEnergyController*       peripheral;
    QLowEnergyService*          service;
    QLowEnergyAdvertisingData   advertisingData;
    QLowEnergyServiceData       serviceData;
};

Peripheral::Peripheral(QObject *parent)
    : QObject(parent),
      d(new PeripheralPrivate(this))
{
    // Advertising will stop once a client connects, so re-advertise once disconnected.
    connect(d->peripheral, &QLowEnergyController::disconnected, this, &Peripheral::startPublishing);
    startPublishing();
}

Peripheral::~Peripheral()
{
    d->peripheral->stopAdvertising();
    d->peripheral->disconnectFromDevice();
    delete d;
}

void Peripheral::startPublishing()
{
    // Publish service
    d->service = d->peripheral->addService(d->serviceData);
    connect(d->service, &QLowEnergyService::characteristicChanged, this, &Peripheral::onCharacteristicChanged);
    d->peripheral->startAdvertising(QLowEnergyAdvertisingParameters(), d->advertisingData/*, d->advertisingData*/);
}

void Peripheral::onCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
{
    // ignore any other characteristic, should not happen
    if (characteristic.uuid() == ble::peqCharacteristicUuid) {
        emit peq(newValue);
    } else if (characteristic.uuid() == ble::crossoverCharacteristicUuid) {
        emit crossover(newValue);
    } else if (characteristic.uuid() == ble::loudnessCharacteristicUuid) {
        emit loudness(newValue);
    }
}

} // namespace ble
