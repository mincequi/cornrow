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

#include "Central.h"

#include "Converter.h"
#include "Defines.h"

#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QLowEnergyAdvertisingParameters>

namespace ble
{

class CentralPrivate : public QObject
{
public:
    CentralPrivate(Central* _q);

    void disconnect();

    // QBluetoothDeviceDiscoveryAgent
    void onDeviceDiscovered(const QBluetoothDeviceInfo&);
    void onDeviceDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error);
    void onDeviceDiscoveryFinished();

    // QLowEnergyController
    void onDeviceConnected();
    void onDeviceDisconnected();
    void onServiceDiscovered(const QBluetoothUuid&);
    void onServiceDiscoveryError(QLowEnergyController::Error);
    void onServiceDiscoveryFinished();

    // QLowEnergyService
    void onServiceStateChanged(QLowEnergyService::ServiceState s);
    void onServiceError(QLowEnergyService::ServiceError error);
    void onCharacteristicRead(const QLowEnergyCharacteristic& characteristic, const QByteArray& value);

    Central* q;
    QBluetoothDeviceDiscoveryAgent* m_discoverer = nullptr;
    QLowEnergyController*   m_control = nullptr;
    QLowEnergyService*      m_service = nullptr;

    Converter m_converter;
};

CentralPrivate::CentralPrivate(Central* _q)
    : q(_q)
{
    m_discoverer = new QBluetoothDeviceDiscoveryAgent(q);
    m_discoverer->setLowEnergyDiscoveryTimeout(5000);

    connect(m_discoverer, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &CentralPrivate::onDeviceDiscovered);
    connect(m_discoverer, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error), this, &CentralPrivate::onDeviceDiscoveryError);
    connect(m_discoverer, &QBluetoothDeviceDiscoveryAgent::finished, this, &CentralPrivate::onDeviceDiscoveryFinished);
}

void CentralPrivate::onDeviceDiscovered(const QBluetoothDeviceInfo &device)
{
    // Only check LE devices
    if (!(device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)) {
        return;
    }

    qDebug() << device.serviceUuids();
    if (!device.serviceUuids().contains(ble::cornrowServiceUuid)) {
        return;
    }

    m_discoverer->stop();

    m_control = new QLowEnergyController(device, this);
    m_control->setRemoteAddressType(QLowEnergyController::PublicAddress);

    // Discover services after device has been connected
    connect(m_control, &QLowEnergyController::serviceDiscovered, this, &CentralPrivate::onServiceDiscovered);
    connect(m_control, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error), this, &CentralPrivate::onServiceDiscoveryError);
    connect(m_control, &QLowEnergyController::discoveryFinished, this, &CentralPrivate::onServiceDiscoveryFinished);

    // Connect to device
    connect(m_control, &QLowEnergyController::connected, this, &CentralPrivate::onDeviceConnected);
    connect(m_control, &QLowEnergyController::disconnected, this, &CentralPrivate::onDeviceDisconnected);
    m_control->connectToDevice();
}

void CentralPrivate::onDeviceDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    qDebug() << __func__ << "error:" << error << "string:" << m_discoverer->errorString();

    q->setStatus(Central::Status::Error, m_discoverer->errorString());
}

void CentralPrivate::onDeviceDiscoveryFinished()
{
    qDebug() << __func__;

    if (!m_control) {
        q->setStatus(Central::Status::Timeout);
    }
}

void CentralPrivate::onDeviceConnected()
{
    qDebug() << "Device connected. Discovering services...";

    q->setStatus(Central::Status::Discovering, "Device connected. Discovering services.");
    m_control->discoverServices();
}

void CentralPrivate::onDeviceDisconnected()
{
    qDebug() << __func__;

    q->disconnect();
    q->setStatus(Central::Status::Lost);
}

void CentralPrivate::onServiceDiscovered(const QBluetoothUuid& serviceUuid)
{
    if (m_service) {
        qDebug() << "Another client is already connected";
        return;
    }
    if (serviceUuid != ble::cornrowServiceUuid) {
        return;
    }

    qDebug() << "Cornrow service discovered...";
    m_service = m_control->createServiceObject(ble::cornrowServiceUuid, this);
    connect(m_service, &QLowEnergyService::stateChanged, this, &CentralPrivate::onServiceStateChanged);
    connect(m_service, &QLowEnergyService::characteristicRead, q, &Central::onCharacteristicRead);
    connect(m_service, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error), this, &CentralPrivate::onServiceError);

    // We cannot access characteristics here, but we have to wait for appropriate state change.
    // discoverDetails() will trigger those state changes.
    m_service->discoverDetails();
}

void CentralPrivate::onServiceDiscoveryError(QLowEnergyController::Error error)
{
    qDebug() << __func__;

    q->setStatus(Central::Status::Error, m_control->errorString());
}

void CentralPrivate::onServiceDiscoveryFinished()
{
    qDebug() << __func__;

    if (m_service) {
        return;
    }

    q->setStatus(Central::Status::Timeout);
}

void CentralPrivate::onServiceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s) {
    case QLowEnergyService::ServiceDiscovered: {
        qDebug() << "Service details discovered.";
        const QLowEnergyCharacteristic peq = m_service->characteristic(ble::peqCharacteristicUuid);
        if (!peq.isValid()) {
            q->disconnect();
            q->setStatus(Central::Status::Error, "Invalid characteristic.");
            return;
        }
        m_service->readCharacteristic(m_service->characteristic(ble::peqCharacteristicUuid));
        break;
    }
    case QLowEnergyService::InvalidService:
        qDebug() << __func__;
        q->setStatus(Central::Status::Lost);
        break;
    case QLowEnergyService::DiscoveryRequired:
        // Rename DiscoveringServices -> DiscoveringDetails or DiscoveringService
    case QLowEnergyService::DiscoveringServices: // discoverDetails() called and running
    case QLowEnergyService::LocalService:   // Only in peripheral role
    default:
        break;
    }
}

void CentralPrivate::onServiceError(QLowEnergyService::ServiceError error)
{
    qDebug() << __func__;

    q->setStatus(Central::Status::Error, "Service error");
}

Central::Central(QObject *parent)
    : QObject(parent),
      d(new CentralPrivate(this))
{
}

Central::~Central()
{
    delete d;
}

bool Central::startDiscovering()
{
    if (d->m_control || d->m_discoverer->isActive()) {
        qDebug() << __func__ << ": already started";
        return false;
    }

    setStatus(Status::Discovering);
    d->m_discoverer->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    return true;
}

void Central::disconnect()
{
    if (d->m_control) {
        d->m_control->disconnectFromDevice();
        delete d->m_control;
        d->m_control = nullptr;
    }
    delete d->m_service;
    d->m_service = nullptr;
}

void Central::writeCharacteristic(common::FilterTask task, const QByteArray &value)
{
    const auto characteristic = d->m_service->characteristic(d->m_converter.toBle(task));
    if (!characteristic.isValid()) {
        qDebug() << __func__ << "Characteristic invalid:" << characteristic.uuid();
        return;
    }
    d->m_service->writeCharacteristic(characteristic, value);
}

void Central::setStatus(Status _error, const QString& errorString)
{
    qDebug() << "Status:" << static_cast<int32_t>(_error) << "error:" << errorString;
    emit status(_error, errorString);
}

void Central::onCharacteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qDebug() << __func__;
    emit status(Status::Connected);
    emit characteristicRead(d->m_converter.fromBle(characteristic.uuid()), value);
}

} // namespace ble
