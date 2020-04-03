#pragma once

#include <QtBluetooth/QLowEnergyAdvertisingParameters>
#include <QtBluetooth/QLowEnergyCharacteristicData>
#include <QtBluetooth/QLowEnergyController>
#include <QtBluetooth/QLowEnergyServiceData>

namespace QZeroProps
{
class BleServer;

class BleServerSession : public QObject
{
    Q_OBJECT

public:
    explicit BleServerSession(BleServer* server, const std::map<QBluetoothUuid, QByteArray>& characteristicsMap);
    ~BleServerSession();

    QLowEnergyController*       peripheral;
    QLowEnergyService*          service;
    QLowEnergyAdvertisingData   advertisingData;
    QLowEnergyServiceData       serviceData;

private:
    void onError();
    void onDisconnected();

    BleServer* m_server = nullptr;
};

} // namespace QZeroProps
