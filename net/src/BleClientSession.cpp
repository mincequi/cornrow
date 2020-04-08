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

#include "BleClientSession.h"

#include <QtGlobal>

#include "QZeroPropsBleClient.h"
#include "QZeroPropsServicePrivate.h"

namespace QtZeroProps
{

BleClientSession::BleClientSession(QZeroPropsServicePrivate* _q)
    : q(_q)
{
}

BleClientSession::~BleClientSession()
{
}

void BleClientSession::connectToDevice(const QBluetoothDeviceInfo& device, const QUuid& serviceUuid)
{
    disconnectFromDevice();

	qDebug() << "Connecting device: " << device.name();

    m_serviceUuid = serviceUuid;

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

void BleClientSession::disconnectFromDevice()
{
    // Disconnect
    if (m_control) {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = nullptr;
    }
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
    emit q->stateChanged(QZeroPropsClient::State::Disconnected);
}

void BleClientSession::onServiceDiscovered(const QBluetoothUuid& serviceUuid)
{
    qDebug() << __func__ << "Service discovered:" << serviceUuid;
}

void BleClientSession::onServiceDiscoveryError(QLowEnergyController::Error /*error*/)
{
    qDebug() << __func__;

    emit q->stateChanged(QZeroPropsClient::State::Error, m_control->errorString());
}

void BleClientSession::onServiceDiscoveryFinished()
{
    qDebug() << __func__;

    if (!m_control->services().contains(m_serviceUuid)) {
        emit q->stateChanged(QZeroPropsClient::State::Error, "Could not find cornrow service on device " + m_control->remoteName());
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
    case QLowEnergyService::ServiceDiscovered:
        for (const auto& charc : m_service->characteristics()) {
            m_service->readCharacteristic(charc);
        }
        break;
    case QLowEnergyService::InvalidService:
        qDebug() << __func__;
        emit q->stateChanged(QZeroPropsClient::State::Disconnected);
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

     emit q->stateChanged(QZeroPropsClient::State::Error, "Service error");
}

void BleClientSession::onCharacteristicRead(const QLowEnergyCharacteristic& characteristic, const QByteArray& value)
{
    qDebug() << __func__ << "> " << characteristic.uuid();
    emit q->stateChanged(QZeroPropsClient::State::Connected);
    emit characteristicChanged(characteristic.uuid().toString(QUuid::StringFormat::WithoutBraces).toStdString().c_str(), value);
}

} // namespace QZeroProps
