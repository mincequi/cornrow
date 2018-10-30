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

#include "Defines.h"

#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QLowEnergyAdvertisingParameters>

namespace ble
{

struct CentralPrivate : public QObject
{
    CentralPrivate(Central* _q)
        : q(_q)
    {
        m_discoverer = new QBluetoothDeviceDiscoveryAgent(q);
        m_discoverer->setLowEnergyDiscoveryTimeout(5000);

        connect(m_discoverer, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &CentralPrivate::onDeviceDiscovered);
        connect(m_discoverer, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error),
        [this] (QBluetoothDeviceDiscoveryAgent::Error error) {
            qDebug() << __func__ << "error:" << error;
        });

        connect(m_discoverer, &QBluetoothDeviceDiscoveryAgent::finished, this, &CentralPrivate::onDeviceDiscoveryFinished);
        //connect(m_discoverer, &QBluetoothDeviceDiscoveryAgent::canceled, this, &CentralPrivateAlt::onDeviceDiscoveryFinished);
    }

    void connectDevice(const QBluetoothDeviceInfo &device);
    void disconnect();

    // QBluetoothDeviceDiscoveryAgent
    void onDeviceDiscovered(const QBluetoothDeviceInfo&);
    void onDeviceDiscoveryFinished();

    // QLowEnergyController
    void onServiceDiscovered(const QBluetoothUuid&);
    void onServiceDiscoveryFinished();

    // QLowEnergyService
    void onServiceStateChanged(QLowEnergyService::ServiceState s);
    void onCharacteristicRead(const QLowEnergyCharacteristic& characteristic, const QByteArray& value);

    Central* q;
    QBluetoothDeviceDiscoveryAgent* m_discoverer = nullptr;
    QLowEnergyController*   m_control = nullptr;
    QLowEnergyService*      m_service = nullptr;
};

void CentralPrivate::connectDevice(const QBluetoothDeviceInfo &device)
{
    m_control = new QLowEnergyController(device, this);
    m_control->setRemoteAddressType(QLowEnergyController::PublicAddress);
    connect(m_control, &QLowEnergyController::serviceDiscovered, this, &CentralPrivate::onServiceDiscovered);
    connect(m_control, &QLowEnergyController::discoveryFinished, this, &CentralPrivate::onServiceDiscoveryFinished);
    connect(m_control, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
    [this] (QLowEnergyController::Error error) {
        //qDebug() << __LINE__;
        q->setError(Central::Error::NoService);
    });

    connect(m_control, &QLowEnergyController::connected, [this]() {
        qDebug() << "Device connected. Discovering services...";
        m_control->discoverServices();
    });
    connect(m_control, &QLowEnergyController::disconnected, this, [this]() {
        //qDebug() << __LINE__;
        q->setError(Central::Error::NoService);
    });

    // Connect
    m_control->connectToDevice();
}

void CentralPrivate::onDeviceDiscovered(const QBluetoothDeviceInfo &device)
{
    // Only check LE devices
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        qDebug() << device.serviceUuids();
        if (!device.serviceUuids().contains(ble::cornrowServiceUuid)) {
            return;
        }
        m_discoverer->stop();
        connectDevice(device);
    }
}

void CentralPrivate::onDeviceDiscoveryFinished()
{
    if (!m_control) {
        qDebug() << __func__;
        q->setError(Central::Error::NoService);
    }
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
    connect(m_service, &QLowEnergyService::characteristicRead, q, &Central::characteristicRead);
    connect(m_service, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error),
    [this] (QLowEnergyService::ServiceError error) {
        qDebug() << __func__;
        q->setError(Central::Error::NoService);
    });

    // We cannot access characteristics here, but we have to wait for appropriate state change.
    // discoverDetails() will trigger those state changes.
    m_service->discoverDetails();
}

void CentralPrivate::onServiceDiscoveryFinished()
{
    if (m_service) {
        return;
    }
    qDebug() << __func__;
    q->setError(Central::Error::NoService);
}

void CentralPrivate::onServiceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s) {
    case QLowEnergyService::ServiceDiscovered: {
        qDebug() << "Service details discovered.";
        const QLowEnergyCharacteristic peq = m_service->characteristic(ble::peqCharacteristicUuid);
        if (!peq.isValid()) {
            q->setError(Central::Error::InvalidCharacteristic);
            return;
        }
        m_service->readCharacteristic(m_service->characteristic(ble::peqCharacteristicUuid));
        break;
    }
    case QLowEnergyService::InvalidService:
        qDebug() << __func__;
        q->setError(Central::Error::NoService);
        break;
    case QLowEnergyService::DiscoveryRequired:
        // Rename DiscoveringServices -> DiscoveringDetails or DiscoveringService
    case QLowEnergyService::DiscoveringServices: // discoverDetails() called and running
    case QLowEnergyService::LocalService:   // Only in peripheral role
    default:
        break;
    }
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

    d->m_discoverer->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    return true;
}

void Central::disconnect()
{
    if (d->m_service) {
        delete d->m_service;
        d->m_service = nullptr;
    }

    if (d->m_control) {
        d->m_control->disconnectFromDevice();
        delete d->m_control;
        d->m_control = nullptr;
    }
}

void Central::setError(Error _error)
{
    disconnect();

    qDebug() << "Error:" << static_cast<int32_t>(_error);
    emit error(_error);
}

} // namespace ble
