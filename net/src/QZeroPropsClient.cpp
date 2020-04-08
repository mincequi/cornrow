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

#include <QtZeroProps/QZeroPropsClient.h>
#include <QtZeroProps/QZeroPropsService.h>

#include "QZeroPropsBleClient.h"
#include "QZeroPropsBleService.h"
#include "QZeroPropsWsService.h"

#include <QUuid>

#include <msgpack.h>
#include <qzeroconf.h>

namespace QtZeroProps
{
using QZeroPropsServicePtr = QSharedPointer<QZeroPropsService>;

class QZeroPropsClientPrivate
{
public:
    QZeroPropsClientPrivate(QZeroPropsClient* _q)
        : q(_q)
    {
        // Setup zeroconf
        QObject::connect(&zeroConf, &QZeroConf::serviceAdded, [this](QZeroConfService service) {
            onServiceDiscovered(service);
        });

        QObject::connect(&bleClient, &QtZeroProps::QZeroPropsBleClient::serviceDiscovered, [this](const QBluetoothDeviceInfo& service,  const QUuid& serviceUuid) {
            onServiceDiscovered(service, serviceUuid);
        });
        QObject::connect(&bleClient, &QZeroPropsBleClient::stateChanged, [this](QZeroPropsClient::State state, const QString& errorString) {
            if (state == QZeroPropsClient::State::Idle) {
                q->stopDiscovery();
                return;
            }
            emit q->stateChanged(state, errorString);
        });
    }

    void onServiceDiscovered(QZeroConfService service)
    {
        // Create impl and attach to base class
        QZeroPropsServicePtr device(new QZeroPropsService);
        auto impl = new QZeroPropsWsService(device.data());
        impl->name = service->name();
        impl->type = QZeroPropsService::ServiceType::WebSocket;
        impl->address = service->ip();
        impl->port = service->port();
        device->d = impl;

        services.push_back(device);
        emit q->servicesChanged();
    }

    void onServiceDiscovered(const QBluetoothDeviceInfo& info, const QUuid& serviceUuid)
    {
        QZeroPropsServicePtr device(new QZeroPropsService);
        QString name = info.name();
        if (name.isEmpty()) {
            name = "<unknown cornrow device>";
        }
        auto impl = new QZeroPropsBleService(device.data());
        impl->name = name;
        impl->type = QZeroPropsService::ServiceType::BluetoothLe;
        impl->bluetoothDeviceInfo = info;
        impl->serviceUuid = serviceUuid;
        device->d = impl;

        services.push_back(device);
        emit q->servicesChanged();
    }

    QZeroPropsClient* const q;

    // ZeroConf members
    QZeroConf           zeroConf;
    QZeroPropsBleClient bleClient;

    QList<QZeroPropsServicePtr> services;
    QZeroPropsService* currentService = nullptr;
};

QZeroPropsClient::QZeroPropsClient(QObject* parent)
    : QObject(parent),
      d(new QZeroPropsClientPrivate(this))
{
}

QZeroPropsClient::~QZeroPropsClient()
{
}

void QZeroPropsClient::setDiscoveryTimeout(int msTimeout)
{
    d->bleClient.setDiscoveryTimeout(msTimeout);
}

QObjectList QZeroPropsClient::discoveredServices() const
{
    QObjectList _devices;
    for (const auto& device : d->services) {
        _devices.push_back(device.get());
    }
    return _devices;
}

void QZeroPropsClient::startDiscovery(const ServiceConfiguration& config)
{
    // Clear previous services
    d->services.clear();
    emit servicesChanged();
    emit stateChanged(State::Discovering);

    if (!config.zeroConfType.isEmpty()) {
        d->zeroConf.startBrowser(config.zeroConfType);
    }

    if (!config.bleUuid.isNull()) {
        d->bleClient.startDiscovery(config.bleUuid);
    }
}

void QZeroPropsClient::stopDiscovery()
{
    d->zeroConf.stopBrowser();
    d->bleClient.stopDiscovery();

    emit stateChanged(State::Idle);
}

void QZeroPropsClient::connectToService(QZeroPropsService* service)
{
    disconnectFromService();

    if (!service) {
        return;
    }

    stopDiscovery();
    emit stateChanged(State::Connecting, "Connecting " + service->name());

    d->currentService = service;
    connect(d->currentService->d, &QZeroPropsServicePrivate::stateChanged, this, &QZeroPropsClient::stateChanged);
    d->currentService->d->connect();
}

void QZeroPropsClient::disconnectFromService()
{
    if (d->currentService) {
        //d->currentService->d->disconnect();
        d->currentService = nullptr;
    }
}

} // namespace QZeroProps
