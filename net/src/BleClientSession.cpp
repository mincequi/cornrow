#include "BleClientSession.h"

#include <QtGlobal>

#include <QZeroProps/QZeroPropsBluetoothLeService.h>
#include "Defines.h"

#include <common/ble/Types.h>

namespace QZeroProps
{

BleClientSession::BleClientSession(const QUuid& uuid, QZeroPropsBluetoothLeService* _q)
    : q(_q),
      m_serviceUuid(uuid)
{
    m_discoverer = new QBluetoothDeviceDiscoveryAgent(q);

    connect(m_discoverer, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BleClientSession::onDeviceDiscovered);
    connect(m_discoverer, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error), this, &BleClientSession::onDeviceDiscoveryError);
    connect(m_discoverer, &QBluetoothDeviceDiscoveryAgent::finished, this, &BleClientSession::onDeviceDiscoveryFinished);

    m_discoverer->setLowEnergyDiscoveryTimeout(8000);
    m_discoverer->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BleClientSession::connectDevice(const QBluetoothDeviceInfo& device)
{
	qDebug() << "Connecting device: " << device.name();

    m_discoverer->stop();
    
    // Disconnect
    if (m_control) {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = nullptr;
    }

    // @TODO(mawe): handle multiple devices
    m_control = new QLowEnergyController(device, this);
    m_control->setRemoteAddressType(QLowEnergyController::PublicAddress);

    // Connect to device
    //q->setStatus(BleClient::Status::Connecting, "Connecting " + m_control->remoteName());

    connect(m_control, &QLowEnergyController::connected, this, &BleClientSession::onDeviceConnected);
    connect(m_control, &QLowEnergyController::disconnected, this, &BleClientSession::onDeviceDisconnected);
    m_control->connectToDevice();

    // Discover services after device has been connected
    connect(m_control, &QLowEnergyController::serviceDiscovered, this, &BleClientSession::onServiceDiscovered);
    connect(m_control, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error), this, &BleClientSession::onServiceDiscoveryError);
    connect(m_control, &QLowEnergyController::discoveryFinished, this, &BleClientSession::onServiceDiscoveryFinished);
}

void BleClientSession::onDeviceDiscovered(const QBluetoothDeviceInfo& device)
{
    // Only check LE devices
    if (!(device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)) {
        return;
    }

    if (!device.serviceUuids().contains(m_serviceUuid)) {
        return;
    }

    qDebug() << __func__ << ": found cornrow device:" << device.name();
    q->setStatus(QZeroPropsClient::State::Discovering);
    m_devices.push_back(device);
    
    emit q->deviceDiscovered(device);
}

void BleClientSession::onDeviceDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    qDebug() << __func__ << "error:" << error << "string:" << m_discoverer->errorString();

    q->setStatus(QZeroPropsClient::State::Error, m_discoverer->errorString());
}

void BleClientSession::onDeviceDiscoveryFinished()
{
    qDebug() << __func__;

    q->setStatus(QZeroPropsClient::State::Idle);
}

void BleClientSession::onDeviceConnected()
{
    qDebug() << "Device connected. Discovering services...";

    m_control->discoverServices();
}

void BleClientSession::onDeviceDisconnected()
{
    qDebug() << __func__;

    q->disconnect();
    q->setStatus(QZeroPropsClient::State::Disconnected);
}

void BleClientSession::onServiceDiscovered(const QBluetoothUuid& serviceUuid)
{
    qDebug() << __func__ << "Service discovered:" << serviceUuid;
}

void BleClientSession::onServiceDiscoveryError(QLowEnergyController::Error /*error*/)
{
    qDebug() << __func__;

    q->setStatus(QZeroPropsClient::State::Error, m_control->errorString());
}

void BleClientSession::onServiceDiscoveryFinished()
{
    qDebug() << __func__;

    if (!m_control->services().contains(m_serviceUuid)) {
        q->setStatus(QZeroPropsClient::State::Error, "Could not find cornrow service on device " + m_control->remoteName());
        return;
    }
    m_service = m_control->createServiceObject(m_serviceUuid, this);
    connect(m_service, &QLowEnergyService::stateChanged, this, &BleClientSession::onServiceStateChanged);
    connect(m_service, &QLowEnergyService::characteristicRead, this, &BleClientSession::onCharacteristicRead);
    connect(m_service, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error), this, &BleClientSession::onServiceError);

    // We cannot access characteristics here, but we have to wait for appropriate state change.
    // discoverDetails() will trigger those state changes.
    m_service->discoverDetails();
    //q->setStatus(BleClient::Status::Connecting, "Reading settings from " + m_control->remoteName());
}

void BleClientSession::onServiceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s) {
    case QLowEnergyService::ServiceDiscovered: {
        qDebug() << "Service details discovered.";
        const QLowEnergyCharacteristic peq = m_service->characteristic(common::peqCharacteristicUuid);
        if (!peq.isValid()) {
            q->disconnect();
            q->setStatus(QZeroPropsClient::State::Error, "Invalid PEQ characteristic");
            return;
        }
        m_service->readCharacteristic(m_service->characteristic(common::peqCharacteristicUuid));

        const QLowEnergyCharacteristic aux = m_service->characteristic(common::auxCharacteristicUuid);
        if (!aux.isValid()) {
            q->disconnect();
            q->setStatus(QZeroPropsClient::State::Error, "Invalid AUX characteristic");
            return;
        }
        m_service->readCharacteristic(m_service->characteristic(common::auxCharacteristicUuid));

        const QLowEnergyCharacteristic caps = m_service->characteristic(QBluetoothUuid(QString::fromStdString(common::ble::ioCapsCharacteristicUuid)));
        if (!caps.isValid()) {
            q->disconnect();
            q->setStatus(QZeroPropsClient::State::Error, "Invalid IoCaps characteristic");
            return;
        }
        m_service->readCharacteristic(m_service->characteristic(QBluetoothUuid(QString::fromStdString(common::ble::ioCapsCharacteristicUuid))));

        const QLowEnergyCharacteristic conf = m_service->characteristic(QBluetoothUuid(QString::fromStdString(common::ble::ioConfCharacteristicUuid)));
        if (!caps.isValid()) {
            q->disconnect();
            q->setStatus(QZeroPropsClient::State::Error, "Invalid IoConf characteristic");
            return;
        }
        m_service->readCharacteristic(m_service->characteristic(QBluetoothUuid(QString::fromStdString(common::ble::ioConfCharacteristicUuid))));

        break;
    }
    case QLowEnergyService::InvalidService:
        qDebug() << __func__;
        q->setStatus(QZeroPropsClient::State::Disconnected);
        break;
    case QLowEnergyService::DiscoveryRequired:
        // Rename DiscoveringServices -> DiscoveringDetails or DiscoveringService
    case QLowEnergyService::DiscoveringServices: // discoverDetails() called and running
    case QLowEnergyService::LocalService:   // Only in peripheral role
        break;
    }
}

void BleClientSession::onServiceError(QLowEnergyService::ServiceError /*error*/)
{
    qDebug() << __func__;

    q->setStatus(QZeroPropsClient::State::Error, "Service error");
}

void BleClientSession::onCharacteristicRead(const QLowEnergyCharacteristic& characteristic, const QByteArray& value)
{
    qDebug() << __func__ << "> " << characteristic.uuid();
    q->setStatus(QZeroPropsClient::State::Connected);
    emit q->characteristicChanged(characteristic.uuid().toString(QUuid::StringFormat::WithoutBraces).toStdString().c_str(), value);
}

} // namespace QZeroProps
