#include "BleServerSession.h"

#include "Defines.h"
#include "BleServer.h"

using namespace common;

namespace QZeroProps
{

BleServerSession::BleServerSession(BleServer* server, const std::map<QBluetoothUuid, QByteArray>& characteristicsMap)
    : m_server(server)
{
    // Service data
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(cornrowServiceUuid);

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

    // Advertising data
    advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    advertisingData.setServices({cornrowServiceUuid});

    // Advertising will stop once a client connects, so re-advertise once disconnected.
    connect(peripheral, &QLowEnergyController::disconnected, this, &BleServerSession::onDisconnected);
    connect(peripheral, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error), this, &BleServerSession::onError);
    // This has to be connected last, since it destroys the previous ServerSession object
    connect(peripheral, &QLowEnergyController::disconnected, server, &BleServer::startPublishing);

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
