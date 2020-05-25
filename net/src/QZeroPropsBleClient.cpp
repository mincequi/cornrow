/*
 * Copyright (C) 2018 Manuel Weichselbaumer <mincequi@web.de>
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

#include "QZeroPropsBleClient.h"

#include "BleClientSession.h"

#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtBluetooth/QLowEnergyAdvertisingParameters>

#include "QZeroPropsBleService.h"

namespace QtZeroProps
{

QZeroPropsBleClient::QZeroPropsBleClient(QObject* parent)
    : QObject(parent)
{
}

QZeroPropsBleClient::~QZeroPropsBleClient()
{
    //delete m_clientSession;
}

void QZeroPropsBleClient::setDiscoveryTimeout(int msTimeout)
{
    m_msTimeout = msTimeout;
}

void QZeroPropsBleClient::startDiscovery(const QUuid& uuid)
{
    stopDiscovery();

    m_serviceUuid = uuid;
    m_discoverer = new QBluetoothDeviceDiscoveryAgent(this);
    connect(m_discoverer, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &QZeroPropsBleClient::onDeviceDiscovered);
    connect(m_discoverer, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error), this, &QZeroPropsBleClient::onDeviceDiscoveryError);
    connect(m_discoverer, &QBluetoothDeviceDiscoveryAgent::finished, this, &QZeroPropsBleClient::onDeviceDiscoveryFinished);

    m_discoverer->setLowEnergyDiscoveryTimeout(m_msTimeout);
    m_discoverer->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void QZeroPropsBleClient::stopDiscovery()
{
    if (m_discoverer) {
        m_discoverer->stop();
        m_discoverer->deleteLater();
        m_discoverer = nullptr;
    }
}

void QZeroPropsBleClient::setStatus(QZeroPropsClient::State _error, const QString& errorString)
{
    qDebug() << "Status:" << static_cast<int32_t>(_error) << "error:" << errorString;
    emit stateChanged(_error, errorString);
}

void QZeroPropsBleClient::onDeviceDiscovered(const QBluetoothDeviceInfo& device)
{
    // Only check LE devices
    if (!(device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)) {
        return;
    }

    // Check if service is already discovered
    auto serviceUuids = device.serviceUuids();
    if (serviceUuids.contains(m_serviceUuid)) {
        qDebug() << __func__ << ": found cornrow device:" << device.name();
        emit serviceDiscovered(device, m_serviceUuid);
        return;
    }

    // We have to do a full discovery
    // @TODO(mawe): this causes crashed. To be fixed. Currently, we rely on services provided from device discovery instead of doing a full discovery.
    //discoverServices(device);
}

void QZeroPropsBleClient::onDeviceDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    qDebug() << __func__ << "error:" << error << "string:" << m_discoverer->errorString();

    setStatus(QZeroPropsClient::State::Error, m_discoverer->errorString());
}

void QZeroPropsBleClient::onDeviceDiscoveryFinished()
{
    qDebug() << __func__;

    setStatus(QZeroPropsClient::State::Idle);
}

void QZeroPropsBleClient::discoverServices(const QBluetoothDeviceInfo& device)
{
    qDebug() << "Connecting device: " << device.name();

    m_controller = new QLowEnergyController(device, this);
    m_controller->setRemoteAddressType(QLowEnergyController::PublicAddress);

    connect(m_controller, &QLowEnergyController::connected, [&]() {
        connect(m_controller, &QLowEnergyController::serviceDiscovered, [&] (const QBluetoothUuid& newService) {
            if (newService == m_serviceUuid) {
                emit serviceDiscovered(device, m_serviceUuid);
                m_controller->disconnectFromDevice();
            }
        });
        connect(m_controller, &QLowEnergyController::discoveryFinished, [this] () {
            m_controller->disconnectFromDevice();
        });

        m_controller->discoverServices();
    });
    connect(m_controller, &QLowEnergyController::disconnected, [this]() {
        m_controller->deleteLater();
    });
    m_controller->connectToDevice();
}

} // namespace QZeroProps
