#include "DeviceModel.h"

#include <QZeroProps/QZeroPropsService.h>

#include <common/ble/Types.h>

#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QGuiApplication>

DeviceModel* DeviceModel::s_instance = nullptr;

DeviceModel* DeviceModel::instance()
{
    return s_instance;
}

DeviceModel* DeviceModel::init(QZeroProps::QZeroPropsClient* netClient)
{
    if (s_instance) {
        return s_instance;
    }

    s_instance = new DeviceModel(netClient);
    return s_instance;
}

DeviceModel::DeviceModel(QZeroProps::QZeroPropsClient* netClient, QObject *parent) :
    QObject(parent),
    m_zpClient(netClient)
{
    connect(m_zpClient, &QZeroProps::QZeroPropsClient::stateChanged, this, &DeviceModel::onDeviceStatus);
    connect(m_zpClient, &QZeroProps::QZeroPropsClient::servicesChanged, this, &DeviceModel::onDevicesChanged);

    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &DeviceModel::onAppStateChanged);
}

void DeviceModel::startDiscovering()
{
    m_zpClient->startDiscovery({ "_cornrow._tcp", QUuid(common::ble::cornrowServiceUuid.c_str())});
}

void DeviceModel::startDemo()
{
    onDeviceStatus(QZeroProps::QZeroPropsClient::State::Connected);
}

QZeroProps::QZeroPropsClient::State DeviceModel::status() const
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
    onDeviceStatus(QZeroProps::QZeroPropsClient::State::Connecting, "Connecting " + service->name());
    m_zpClient->connectToService(service);
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

void DeviceModel::onDeviceStatus(QZeroProps::QZeroPropsClient::State _status, const QString& statusText)
{
    m_statusText.clear();

    switch (_status) {
    /*
    case QZeroProps::QZeroPropsClient::State::NoBluetooth:
        m_statusLabel = "Bluetooth disabled";
        m_statusText = "Enable Bluetooth in your device's settings";
        break;
        */
    case QZeroProps::QZeroPropsClient::State::Idle:
        if (m_status == QZeroProps::QZeroPropsClient::State::Connected) {
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
    case QZeroProps::QZeroPropsClient::State::Discovering:
        if (m_zpClient->discoveredServices().empty()) {
            m_statusLabel = "Discovering";
        } else {
            m_statusLabel = "Discovering";
            m_statusText = "Cornrow devices found. Tap to connect";
        }
        break;
    case QZeroProps::QZeroPropsClient::State::Connecting:
        m_statusLabel = "Connecting";
        m_statusText = statusText;
        break;
    case QZeroProps::QZeroPropsClient::State::Connected:
        m_statusLabel = "";
        break;
    case QZeroProps::QZeroPropsClient::State::Disconnected:
        m_statusLabel = "Lost";
        m_statusText = "Connection has been interrupted";
        break;
    case QZeroProps::QZeroPropsClient::State::Error:
        m_statusLabel = "Error";
        m_statusText = statusText;
        break;
    }

    m_status = _status;
    emit statusChanged();
}

void DeviceModel::onDevicesChanged()
{
    emit servicesChanged();
    
    onDeviceStatus(m_status, "");
}
