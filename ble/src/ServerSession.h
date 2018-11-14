#pragma once

#include <QtBluetooth/QLowEnergyAdvertisingParameters>
#include <QtBluetooth/QLowEnergyCharacteristicData>
#include <QtBluetooth/QLowEnergyController>
#include <QtBluetooth/QLowEnergyServiceData>

namespace ble
{
class Server;

class ServerSession : public QObject
{
    Q_OBJECT

public:
    explicit ServerSession(Server* server, const std::map<QBluetoothUuid, QByteArray>& characteristicsMap);
    ~ServerSession();

    QLowEnergyController*       peripheral;
    QLowEnergyService*          service;
    QLowEnergyAdvertisingData   advertisingData;
    QLowEnergyServiceData       serviceData;

private:
    Server* m_server = nullptr;
};

} // namespace ble
