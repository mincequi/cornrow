/*
 * Copyright (C) 2020 Manuel Weichselbaumer <mincequi@web.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DeviceModel.h"

#include <QtZeroProps/QZeroPropsService.h>
#include <QtZeroProps/QZeroPropsTypes.h>

#include <common/ble/Types.h>

#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QGuiApplication>
#include <QUuid>

DeviceModel* DeviceModel::s_instance = nullptr;

DeviceModel* DeviceModel::instance()
{
    return s_instance;
}

DeviceModel* DeviceModel::init(QtZeroProps::QZeroPropsClient* netClient)
{
    if (s_instance) {
        return s_instance;
    }

    s_instance = new DeviceModel(netClient);
    return s_instance;
}

DeviceModel::DeviceModel(QtZeroProps::QZeroPropsClient* netClient, QObject *parent) :
    QObject(parent),
    m_zpClient(netClient)
{
    connect(m_zpClient, &QtZeroProps::QZeroPropsClient::stateChanged, this, &DeviceModel::onDeviceStatus);
    connect(m_zpClient, &QtZeroProps::QZeroPropsClient::servicesChanged, this, &DeviceModel::onDevicesChanged);
    connect(m_zpClient, &QtZeroProps::QZeroPropsClient::connectedServiceChanged, this, &DeviceModel::connectedServiceChanged);

    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &DeviceModel::onAppStateChanged);
}

void DeviceModel::startDiscovering()
{
    m_zpClient->startDiscovery({ "_cornrow._tcp", QUuid(common::ble::cornrowServiceUuid.c_str())});
}

void DeviceModel::startDemo()
{
    onDeviceStatus(QtZeroProps::QZeroPropsClient::State::Connected);
}

QtZeroProps::QZeroPropsClient::State DeviceModel::status() const
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
    QList<QObject*> _devices;
    for (const auto& device : m_zpClient->discoveredServices()) {
        _devices.push_back(device);
    }

    return _devices;
}

QObject* DeviceModel::connectedService() const
{
    return m_zpClient->connectedService();
}

void DeviceModel::connectToService(QtZeroProps::QZeroPropsService* service)
{
    onDeviceStatus(QtZeroProps::QZeroPropsClient::State::Connecting, "Connecting " + service->name());
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

void DeviceModel::onDeviceStatus(QtZeroProps::QZeroPropsClient::State _status, const QString& statusText)
{
    m_statusText.clear();

    switch (_status) {
    /*
    case QZeroProps::QZeroPropsClient::State::NoBluetooth:
        m_statusLabel = "Bluetooth disabled";
        m_statusText = "Enable Bluetooth in your device's settings";
        break;
        */
    case QtZeroProps::QZeroPropsClient::State::Idle:
        if (m_status == QtZeroProps::QZeroPropsClient::State::Connected) {
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
    case QtZeroProps::QZeroPropsClient::State::Discovering:
        if (m_zpClient->discoveredServices().empty()) {
            m_statusLabel = "Discovering";
        } else {
            m_statusLabel = "Discovering";
            m_statusText = "Cornrow devices found. Tap to connect";
        }
        break;
    case QtZeroProps::QZeroPropsClient::State::Connecting:
        m_statusLabel = "Connecting";
        m_statusText = statusText;
        break;
    case QtZeroProps::QZeroPropsClient::State::Connected:
        m_statusLabel = "";
        break;
    case QtZeroProps::QZeroPropsClient::State::Disconnected:
        m_statusLabel = "Lost";
        m_statusText = "Connection has been interrupted";
        break;
    case QtZeroProps::QZeroPropsClient::State::Error:
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
