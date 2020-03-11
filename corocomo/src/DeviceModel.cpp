#include "DeviceModel.h"

#include <net/NetClient.h>

#include <QGuiApplication>

DeviceModel* DeviceModel::s_instance = nullptr;

DeviceModel* DeviceModel::instance()
{
    if (s_instance) {
        return s_instance;
    }

    s_instance = new DeviceModel();
    return s_instance;
}

DeviceModel::DeviceModel(QObject *parent) : QObject(parent)
{
    // Net
    m_netClient = new net::NetClient(this);

    connect(m_netClient, &net::NetClient::deviceDiscovered, this, &DeviceModel::onNetDeviceDiscovered);
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &DeviceModel::onAppStateChanged);
}

void DeviceModel::startDiscovering()
{
    m_netClient->startDiscovering();
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

std::vector<net::NetDevice> DeviceModel::devices() const
{
    return m_devices;
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

void DeviceModel::onNetDeviceDiscovered(const net::NetDevice& device)
{
    m_devices.push_back(device);
    emit devicesChanged();
}
