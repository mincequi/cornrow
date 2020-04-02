#include "DeviceModel.h"

#include <ble/BleClient.h>
#include <QZeroProps/QZeroPropsService.h>

#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QBluetoothUuid>
#include <QGuiApplication>

#include "BleCentralAdapter.h"

DeviceModel* DeviceModel::s_instance = nullptr;

DeviceModel* DeviceModel::instance()
{
    return s_instance;
}

DeviceModel* DeviceModel::init(BleCentralAdapter* bleAdapter, QZeroProps::QZeroPropsClient* netClient)
{
    if (s_instance) {
        return s_instance;
    }

    s_instance = new DeviceModel(bleAdapter, netClient);
    return s_instance;
}

DeviceModel::DeviceModel(BleCentralAdapter* bleAdapter, QZeroProps::QZeroPropsClient* netClient, QObject *parent) :
    QObject(parent),
    m_bleAdapter(bleAdapter),
    m_tcpClient(netClient)
{
    // Ble
    connect(m_bleAdapter, &BleCentralAdapter::status, this, &DeviceModel::onBleDeviceStatus);
    connect(m_bleAdapter, &BleCentralAdapter::deviceDiscovered, this, &DeviceModel::onBleDeviceDiscovered);

    // Net
    connect(m_tcpClient, &QZeroProps::QZeroPropsClient::stateChanged, this, &DeviceModel::onNetDeviceStatus);
    connect(m_tcpClient, &QZeroProps::QZeroPropsClient::deviceDiscovered, this, &DeviceModel::onNetDeviceDiscovered);
    connect(m_tcpClient, &QZeroProps::QZeroPropsClient::deviceDisappeared, this, &DeviceModel::onNetDeviceDisappeared);
    
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &DeviceModel::onAppStateChanged);
}

void DeviceModel::startDiscovering()
{
    // @TODO(mawe): Only clear BLE devices, since QtZeroConf has an issue when re-discovering.
    // m_devices.clear();
    for (auto it = m_devices.begin(); it != m_devices.end(); ++it) {
        if ((*it)->m_type == QZeroProps::QZeroPropsService::Type::BluetoothLe) {
            it = m_devices.erase(it);
            if (it == m_devices.end()) {
                break;
            }
        }
    }
    emit devicesChanged();

    m_bleAdapter->startDiscovering();
    m_tcpClient->startDiscovery({ "_cornrow._tcp", QUuid()});
    onBleDeviceStatus(Status::Discovering);
}

void DeviceModel::startDemo()
{
    onBleDeviceStatus(Status::Connected, QString());
}

DeviceModel::Status DeviceModel::status() const
{
    return m_status;
}

QString DeviceModel::statusLabel() const
{
    return m_statusLabel;
}

QString DeviceModel::statusText() const
{
    return m_statusText;
}

QObjectList DeviceModel::devices() const
{
    QObjectList _devices;
    for (const auto& device : m_devices) {
        _devices.push_back(device.get());
    }
    return _devices;
}

void DeviceModel::connectToService(QZeroProps::QZeroPropsService* service)
{
    switch (service->m_type) {
    case QZeroProps::QZeroPropsService::Type::BluetoothLe:
        onBleDeviceStatus(Status::Connecting, "Connecting " + service->m_name);
        m_bleAdapter->connectDevice(service->m_bluetoothDeviceInfo);
        break;
    case QZeroProps::QZeroPropsService::Type::WebSocket:
        onBleDeviceStatus(Status::Connecting, "Connecting " + service->m_name);
        m_tcpClient->connectToService(service);
        break;
    default:
        qDebug() << "Unhandled device type: " << service->m_type;
        break;
    }
}

void DeviceModel::onAppStateChanged(Qt::ApplicationState state)
{
    switch (state) {
    case Qt::ApplicationSuspended:
        m_tcpClient->stopDiscovery();
        break;
    case Qt::ApplicationHidden:
    case Qt::ApplicationInactive:
    case Qt::ApplicationActive:
        break;
    }
}

void DeviceModel::onBleDeviceStatus(Status _status, const QString& statusText)
{
    m_statusText.clear();

    switch (_status) {
    case Status::NoBluetooth:
        m_statusLabel = "Bluetooth disabled";
        m_statusText = "Enable Bluetooth in your device's settings";
        break;
    case Status::Idle:
        if (m_status == Status::Connected) {
            return;
        }
        //m_tcpClient->stopDiscovering();
        if (m_devices.empty()) {
            m_statusLabel = "Timeout";
            m_statusText = "Be sure to be close to a cornrow device";
        } else {
            m_statusLabel = "";
            m_statusText = "Cornrow devices found. Tap to connect";
        }
        break;
    case Status::Discovering:
        if (m_devices.empty()) {
            m_statusLabel = "Discovering";
        } else {
            m_statusLabel = "Discovering";
            m_statusText = "Cornrow devices found. Tap to connect";
        }
        break;
    case Status::Connecting:
        m_statusLabel = "Connecting";
        m_statusText = statusText;
        break;
    case Status::Connected:
        m_statusLabel = "";
        break;
    case Status::Lost:
        m_statusLabel = "Lost";
        m_statusText = "Connection has been interrupted";
        break;
    case Status::Error:
        m_statusLabel = "Error";
        m_statusText = statusText;
        break;
    }

    m_status = _status;
    emit statusChanged();
}

void DeviceModel::onNetDeviceStatus(QZeroProps::QZeroPropsClient::State _status, const QString& statusText)
{
    switch (_status) {
    case QZeroProps::QZeroPropsClient::State::Idle:
        onBleDeviceStatus(DeviceModel::Status::Idle);
        return;
    case QZeroProps::QZeroPropsClient::State::Discovering:
        onBleDeviceStatus(DeviceModel::Status::Discovering);
        return;
    case QZeroProps::QZeroPropsClient::State::Connecting:
        onBleDeviceStatus(DeviceModel::Status::Connecting, statusText);
        return;
    case QZeroProps::QZeroPropsClient::State::Connected:
        onBleDeviceStatus(DeviceModel::Status::Connected);
        return;
    case QZeroProps::QZeroPropsClient::State::Disconnected:
        onBleDeviceStatus(DeviceModel::Status::Lost);
        return;
    case QZeroProps::QZeroPropsClient::State::Error:
        onBleDeviceStatus(DeviceModel::Status::Error, statusText);
        return;
    }
}

void DeviceModel::onBleDeviceDiscovered(const QBluetoothDeviceInfo& _device)
{
    QZeroProps::QZeroPropsServicePtr device(new QZeroProps::QZeroPropsService);
    QString name = _device.name();
    if (name.isEmpty()) {
        name = "<unknown cornrow device>";
    }
    device->m_name = name;
    device->m_type = QZeroProps::QZeroPropsService::Type::BluetoothLe;
    device->m_bluetoothDeviceInfo = _device;

    m_devices.push_back(device);
    emit devicesChanged();
    
    if (m_status == Status::Discovering) {
        onBleDeviceStatus(Status::Discovering, "");
    }
}

void DeviceModel::onNetDeviceDiscovered(QZeroProps::QZeroPropsServicePtr device)
{
    m_devices.push_back(device);
    emit devicesChanged();
    
    onBleDeviceStatus(m_status, "");
}

void DeviceModel::onNetDeviceDisappeared(const QHostAddress& address)
{
    for (auto it = m_devices.begin(); it != m_devices.end(); ++it) {
        if ((*it)->m_address == address) {
            it = m_devices.erase(it);
            if (it == m_devices.end()) {
                break;
            }
        }
    }

    emit devicesChanged();

    onBleDeviceStatus(m_status, "");
}
