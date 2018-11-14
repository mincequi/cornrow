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

    // Advertising data
    advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    advertisingData.setServices({cornrowServiceUuid});

    peripheral->startAdvertising(QLowEnergyAdvertisingParameters(), advertisingData/*, advertisingData*/);
}

ServerSession::~ServerSession()
{
    peripheral->stopAdvertising();
    peripheral->disconnectFromDevice();
}

} // namespace ble
