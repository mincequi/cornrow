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

#include "Server.h"

#include "Defines.h"

#include <QtBluetooth/QLowEnergyAdvertisingParameters>
#include <QtBluetooth/QLowEnergyCharacteristicData>
#include <QtBluetooth/QLowEnergyController>
#include <QtBluetooth/QLowEnergyServiceData>

namespace ble
{

class PeripheralPrivate
{
public:
    PeripheralPrivate()
    {
        advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
        advertisingData.setServices({cornrowServiceUuid});

        // Service
        serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
        serviceData.setUuid(cornrowServiceUuid);
    }

    QLowEnergyController*       peripheral;
    QLowEnergyService*          service;
    QLowEnergyAdvertisingData   advertisingData;
    QLowEnergyServiceData       serviceData;
};

Peripheral::Peripheral(QObject *parent)
    : QObject(parent),
      d(new PeripheralPrivate())
{
}

Peripheral::~Peripheral()
{
    d->peripheral->stopAdvertising();
    d->peripheral->disconnectFromDevice();
    delete d;
}

void Peripheral::init(const std::map<QBluetoothUuid, QByteArray>& characteristicsMap)
{
    QList<QLowEnergyCharacteristicData> characteristics;
    for (const auto& kv : characteristicsMap) {
        QLowEnergyCharacteristicData characteristicData;
        characteristicData.setUuid(kv.first);
        characteristicData.setValue(kv.second);
        characteristicData.setProperties(QLowEnergyCharacteristic::Read | QLowEnergyCharacteristic::Write);
        characteristics << characteristicData;
    }
    d->serviceData.setCharacteristics(characteristics);

    d->peripheral = QLowEnergyController::createPeripheral(this);
    d->service = d->peripheral->addService(d->serviceData);
    connect(d->service, &QLowEnergyService::characteristicChanged, this, &Peripheral::characteristicChanged);

    // Advertising will stop once a client connects, so re-advertise once disconnected.
    startPublishing();
    connect(d->peripheral, &QLowEnergyController::disconnected, this, &Peripheral::startPublishing);
}

void Peripheral::startPublishing()
{
    // Publish service
    d->peripheral->startAdvertising(QLowEnergyAdvertisingParameters(), d->advertisingData/*, d->advertisingData*/);
}

} // namespace ble
