#include "DeviceModel.h"

#include <ble/BleClient.h>
#include <net/NetClient.h>

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

DeviceModel* DeviceModel::init(BleCentralAdapter* bleAdapter)
{
    if (s_instance) {
        return s_instance;
    }

    s_instance = new DeviceModel(bleAdapter);
    return s_instance;
}

DeviceModel::DeviceModel(BleCentralAdapter* bleAdapter, QObject *parent) :
	QObject(parent),
	m_bleAdapter(bleAdapter)
{
    connect(m_bleAdapter, &BleCentralAdapter::status, this, &DeviceModel::onDeviceStatus);

	// Ble
	connect(m_bleAdapter, &BleCentralAdapter::deviceDiscovered, this, &DeviceModel::onBleDeviceDiscovered);
	
    // Net
    m_netClient = new net::NetClient(this);
    connect(m_netClient, &net::NetClient::deviceDiscovered, this, &DeviceModel::onNetDeviceDiscovered);
    
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &DeviceModel::onAppStateChanged);
}

void DeviceModel::startDiscovering()
{
	m_devices.clear();
	emit devicesChanged();
	
	m_bleAdapter->startDiscovering();
    m_netClient->startDiscovering();
}

void DeviceModel::startDemo()
{
    onDeviceStatus(Status::Connected, QString());
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

void DeviceModel::connectDevice(net::NetDevice* device)
{
    switch (device->type) {
    case net::NetDevice::DeviceType::BluetoothLe:
        m_bleAdapter->connectDevice(device->bluetoothDeviceInfo);
        break;
    default:
        qDebug() << "Unhandled device type: " << device->type;
        break;
    }
}

void DeviceModel::onAppStateChanged(Qt::ApplicationState state)
{
    switch (state) {
    case Qt::ApplicationSuspended:
    case Qt::ApplicationHidden:
    case Qt::ApplicationInactive:
        m_netClient->stopDiscovering();
        break;
    case Qt::ApplicationActive:
        break;
    }
}

void DeviceModel::onDeviceStatus(Status _status, const QString& statusText)
{
    m_status = _status;
    m_statusText.clear();

    switch (_status) {
    case Status::NoBluetooth:
        m_statusLabel = "Bluetooth disabled";
        m_statusText = "Enable Bluetooth in your device's settings";
        break;
    case Status::Idle:
        if (m_devices.empty()) {
            m_statusLabel = "Timeout";
            m_statusText = "Be sure to be close to a cornrow device";
        } else {
            m_statusLabel = "";
            m_statusText = "Cornrow devices found. Tap to connect:";
        }
        break;
    case Status::Discovering:
        m_statusLabel = "Discovering";
        m_statusText = statusText;
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

    emit statusChanged();
}

void DeviceModel::onBleDeviceDiscovered(const QBluetoothDeviceInfo& _device)
{
	net::NetDevicePtr device(new net::NetDevice);
	QString name = _device.name();
	if (name.isEmpty()) {
		name = "<unknown cornrow device>";
	}
    device->name = name;
    device->type = net::NetDevice::DeviceType::BluetoothLe;
    device->bluetoothDeviceInfo = _device;

	m_devices.push_back(device);
    emit devicesChanged();
}

void DeviceModel::onNetDeviceDiscovered(net::NetDevicePtr device)
{
    m_devices.push_back(device);
    emit devicesChanged();
}
