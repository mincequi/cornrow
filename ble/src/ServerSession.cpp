#include "ServerSession.h"

#include "Defines.h"
#include "Server.h"

namespace ble
{

ServerSession::ServerSession(Server* server, const std::map<QBluetoothUuid, QByteArray>& characteristicsMap)
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
    connect(service, &QLowEnergyService::characteristicChanged, m_server, &Server::characteristicChanged);
    connect(service, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error), this, &ServerSession::onError);

    // Advertising data
    advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    advertisingData.setServices({cornrowServiceUuid});

    // Advertising will stop once a client connects, so re-advertise once disconnected.
    connect(peripheral, &QLowEnergyController::disconnected, this, &ServerSession::onDisconnected);
    connect(peripheral, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error), this, &ServerSession::onError);
    // This has to be connected last, since it destroys the previous ServerSession object
    connect(peripheral, &QLowEnergyController::disconnected, server, &Server::startPublishing);

    peripheral->startAdvertising(QLowEnergyAdvertisingParameters(), advertisingData/*, advertisingData*/);
}

ServerSession::~ServerSession()
{
    peripheral->stopAdvertising();
    peripheral->disconnectFromDevice();
}

void ServerSession::onError()
{
    qDebug() << __func__;

    emit m_server->deviceDisconnected();
}

void ServerSession::onDisconnected()
{
    qDebug() << __func__;

    emit m_server->deviceDisconnected();
}

} // namespace ble
