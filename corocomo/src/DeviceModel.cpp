#include "DeviceModel.h"

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
    m_zpClient(netClient)
{
    // Ble
    connect(m_bleAdapter, &BleCentralAdapter::status, this, &DeviceModel::onBleDeviceStatus);
    connect(m_bleAdapter->central(), &ble::BleClient::deviceDiscovered, this, &DeviceModel::onBleDeviceDiscovered);

    // Net
    connect(m_zpClient, &QZeroProps::QZeroPropsClient::stateChanged, this, &DeviceModel::onNetDeviceStatus);
    connect(m_zpClient, &QZeroProps::QZeroPropsClient::servicesChanged, this, &DeviceModel::onDevicesChanged);

    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &DeviceModel::onAppStateChanged);
}

void DeviceModel::startDiscovering()
{
    m_bleAdapter->startDiscovering(QUuid(common::ble::cornrowServiceUuid.c_str()));
    m_zpClient->startDiscovery({ "_cornrow._tcp", QUuid()});
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

QObjectList DeviceModel::services() const
{
    return m_zpClient->discoveredServices();
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
        m_zpClient->connectToService(service);
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
        m_zpClient->stopDiscovery();
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
        //m_zpClient->stopDiscovering();
        if (m_zpClient->discoveredServices().empty()) {
            m_statusLabel = "Timeout";
            m_statusText = "Be sure to be close to a cornrow device";
        } else {
            m_statusLabel = "";
            m_statusText = "Cornrow devices found. Tap to connect";
        }
        break;
    case Status::Discovering:
        if (m_zpClient->discoveredServices().empty()) {
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

    //m_zpClient->discoveredServices().push_back(device);
    //emit servicesChanged();
    
    if (m_status == Status::Discovering) {
        onBleDeviceStatus(Status::Discovering, "");
    }
}

void DeviceModel::onDevicesChanged()
{
    emit servicesChanged();
    
    onBleDeviceStatus(m_status, "");
}
