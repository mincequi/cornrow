#pragma once

#include <QObject>

#include <common/ble/Converter.h>

#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QLowEnergyAdvertisingParameters>

namespace ble
{
class BleClient;

class ClientSession : public QObject
{
public:
    ClientSession(BleClient* _q);

	void connectDevice(const QBluetoothDeviceInfo& device);

    // QBluetoothDeviceDiscoveryAgent
    void onDeviceDiscovered(const QBluetoothDeviceInfo&);
    void onDeviceDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error);
    void onDeviceDiscoveryFinished();

    // QLowEnergyController
    void onDeviceConnected();
    void onDeviceDisconnected();
    void onServiceDiscovered(const QBluetoothUuid&);
    void onServiceDiscoveryError(QLowEnergyController::Error);
    void onServiceDiscoveryFinished();

    // QLowEnergyService
    void onServiceStateChanged(QLowEnergyService::ServiceState s);
    void onServiceError(QLowEnergyService::ServiceError error);
    void onCharacteristicRead(const QLowEnergyCharacteristic& characteristic, const QByteArray& value);

    BleClient* q;
    QBluetoothDeviceDiscoveryAgent* m_discoverer = nullptr;
    QLowEnergyController*   m_control = nullptr;
    QLowEnergyService*      m_service = nullptr;

    common::ble::Converter m_converter;

private:
    std::list<QBluetoothDeviceInfo> m_devices;
};

} // namespace ble
