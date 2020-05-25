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

#include "BleServerSession.h"

#include "BleServer.h"

namespace QtZeroProps
{

BleServerSession::BleServerSession(const QUuid& serviceUuid, BleServer* server, const std::map<QBluetoothUuid, QByteArray>& characteristicsMap)
    : m_serviceUuid(serviceUuid),
      m_server(server)
{
    // Service data
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(m_serviceUuid);

    QList<QLowEnergyCharacteristicData> characteristics;
    for (const auto& kv : characteristicsMap) {
        QLowEnergyCharacteristicData characteristicData;
        characteristicData.setUuid(kv.first);
        characteristicData.setValue(kv.second);
        characteristicData.setProperties(QLowEnergyCharacteristic::Read | QLowEnergyCharacteristic::Write);
        characteristics << characteristicData;
    }
    serviceData.setCharacteristics(characteristics);

    // Peripheral
    peripheral = QLowEnergyController::createPeripheral(this);
    service = peripheral->addService(serviceData);
    connect(service, &QLowEnergyService::characteristicChanged, m_server, &BleServer::characteristicChanged);
    connect(service, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error), this, &BleServerSession::onError);

    // Advertising will stop once a client connects, so re-advertise once disconnected.
    connect(peripheral, &QLowEnergyController::disconnected, this, &BleServerSession::onDisconnected);
    connect(peripheral, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error), this, &BleServerSession::onError);
    // This has to be connected last, since it destroys the previous ServerSession object
    connect(peripheral, &QLowEnergyController::disconnected, [this]() {
        m_server->startPublishing(m_serviceUuid);
    });

    // Advertising data
    advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    advertisingData.setServices({m_serviceUuid});
    peripheral->startAdvertising(QLowEnergyAdvertisingParameters(), advertisingData/*, advertisingData*/);
}

BleServerSession::~BleServerSession()
{
    peripheral->stopAdvertising();
    peripheral->disconnectFromDevice();
}

void BleServerSession::onError()
{
    qDebug() << __func__;

    emit m_server->deviceDisconnected();
}

void BleServerSession::onDisconnected()
{
    qDebug() << __func__;

    emit m_server->deviceDisconnected();
}

} // namespace QZeroProps
