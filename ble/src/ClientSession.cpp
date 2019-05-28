#include "ClientSession.h"

#include <QtGlobal>

#include "Client.h"
#include "Defines.h"

namespace ble
{

ClientSession::ClientSession(Client* _q)
    : q(_q)
{
    m_discoverer = new QBluetoothDeviceDiscoveryAgent(q);

    connect(m_discoverer, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &ClientSession::onDeviceDiscovered);
    connect(m_discoverer, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error), this, &ClientSession::onDeviceDiscoveryError);
    connect(m_discoverer, &QBluetoothDeviceDiscoveryAgent::finished, this, &ClientSession::onDeviceDiscoveryFinished);

    // @TODO(mawe): make Qt 5.7 compatible
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    m_discoverer->setLowEnergyDiscoveryTimeout(8000);
    m_discoverer->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
#else
    m_discoverer->start();
#endif
}

void ClientSession::onDeviceDiscovered(const QBluetoothDeviceInfo &device)
{
    // Only check LE devices
    if (!(device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)) {
        return;
    }
    if (!device.serviceUuids().contains(ble::cornrowServiceUuid)) {
        return;
    }

    qDebug() << __func__ << ": found cornrow device:" << device.name();
    q->setStatus(Client::Status::Discovering, "Found cornrow device " + device.name());
    m_devices.push_back(device);

    // @TODO(mawe): until we do not support multiple devices, we can stop here.
    onDeviceDiscoveryFinished();
}

void ClientSession::onDeviceDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    qDebug() << __func__ << "error:" << error << "string:" << m_discoverer->errorString();

    q->setStatus(Client::Status::Error, m_discoverer->errorString());
}

void ClientSession::onDeviceDiscoveryFinished()
{
    qDebug() << __func__;

    if (m_devices.empty()) {
        q->setStatus(Client::Status::Timeout);
        return;
    }

    m_discoverer->stop();

    // Disconnect
    if (m_control) {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = nullptr;
    }

    // @TODO(mawe): handle multiple devices
    m_control = new QLowEnergyController(m_devices.front(), this);
    m_control->setRemoteAddressType(QLowEnergyController::PublicAddress);

    // Connect to device
    q->setStatus(Client::Status::Connecting, "Connecting " + m_control->remoteName());

    connect(m_control, &QLowEnergyController::connected, this, &ClientSession::onDeviceConnected);
    connect(m_control, &QLowEnergyController::disconnected, this, &ClientSession::onDeviceDisconnected);
    m_control->connectToDevice();

    // Discover services after device has been connected
    connect(m_control, &QLowEnergyController::serviceDiscovered, this, &ClientSession::onServiceDiscovered);
    connect(m_control, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error), this, &ClientSession::onServiceDiscoveryError);
    connect(m_control, &QLowEnergyController::discoveryFinished, this, &ClientSession::onServiceDiscoveryFinished);
}

void ClientSession::onDeviceConnected()
{
    qDebug() << "Device connected. Discovering services...";

    m_control->discoverServices();
}

void ClientSession::onDeviceDisconnected()
{
    qDebug() << __func__;

    q->disconnect();
    q->setStatus(Client::Status::Lost);
}

void ClientSession::onServiceDiscovered(const QBluetoothUuid& serviceUuid)
{
    qDebug() << __func__ << "Service discovered:" << serviceUuid;
}

void ClientSession::onServiceDiscoveryError(QLowEnergyController::Error /*error*/)
{
    qDebug() << __func__;

    q->setStatus(Client::Status::Error, m_control->errorString());
}

void ClientSession::onServiceDiscoveryFinished()
{
    qDebug() << __func__;

    if (!m_control->services().contains(ble::cornrowServiceUuid)) {
        q->setStatus(Client::Status::Error, "Could not find cornrow service on device " + m_control->remoteName());
        return;
    }
    m_service = m_control->createServiceObject(ble::cornrowServiceUuid, this);
    connect(m_service, &QLowEnergyService::stateChanged, this, &ClientSession::onServiceStateChanged);
    connect(m_service, &QLowEnergyService::characteristicRead, q, &Client::onCharacteristicRead);
    connect(m_service, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error), this, &ClientSession::onServiceError);

    // We cannot access characteristics here, but we have to wait for appropriate state change.
    // discoverDetails() will trigger those state changes.
    m_service->discoverDetails();
    q->setStatus(Client::Status::Connecting, "Reading settings from " + m_control->remoteName());
}

void ClientSession::onServiceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s) {
    case QLowEnergyService::ServiceDiscovered: {
        qDebug() << "Service details discovered.";
        const QLowEnergyCharacteristic peq = m_service->characteristic(ble::peqCharacteristicUuid);
        if (!peq.isValid()) {
            q->disconnect();
            q->setStatus(Client::Status::Error, "Invalid PEQ characteristic");
            return;
        }
        m_service->readCharacteristic(m_service->characteristic(ble::peqCharacteristicUuid));

        const QLowEnergyCharacteristic aux = m_service->characteristic(ble::auxCharacteristicUuid);
        if (!aux.isValid()) {
            q->disconnect();
            q->setStatus(Client::Status::Error, "Invalid AUX characteristic");
            return;
        }
        m_service->readCharacteristic(m_service->characteristic(ble::auxCharacteristicUuid));

        break;
    }
    case QLowEnergyService::InvalidService:
        qDebug() << __func__;
        q->setStatus(Client::Status::Lost);
        break;
    case QLowEnergyService::DiscoveryRequired:
        // Rename DiscoveringServices -> DiscoveringDetails or DiscoveringService
    case QLowEnergyService::DiscoveringServices: // discoverDetails() called and running
    case QLowEnergyService::LocalService:   // Only in peripheral role
        break;
    }
}

void ClientSession::onServiceError(QLowEnergyService::ServiceError /*error*/)
{
    qDebug() << __func__;

    q->setStatus(Client::Status::Error, "Service error");
}

} // namespace ble
