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

#include <QZeroProps/QZeroPropsClient.h>
#include <QZeroProps/QZeroPropsService.h>

#include "QZeroPropsServicePrivate.h"

#include <QUuid>

#include <msgpack.h>
#include <qzeroconf.h>

uint qHash(const QVariant& var)
{
    switch (var.type()) {
    case QVariant::Uuid:
        return qHash(var.toUuid());
    case QVariant::UInt:
        return qHash(var.toUInt());
    default:
        return 0;
    }
}

namespace QZeroProps
{

QZeroPropsClient::QZeroPropsClient(QObject* parent)
    : QObject(parent),
      d(nullptr)
{
    // Setup zeroconf
    m_zeroConf = new QZeroConf(this);
    connect(m_zeroConf, &QZeroConf::serviceAdded, this, &QZeroPropsClient::onServiceDiscovered);
    connect(m_zeroConf, &QZeroConf::serviceRemoved, this, &QZeroPropsClient::onServiceRemoved);
}

QZeroPropsClient::~QZeroPropsClient()
{
}

QObjectList QZeroPropsClient::discoveredServices() const
{
    QObjectList _devices;
    for (const auto& device : m_services) {
        _devices.push_back(device.get());
    }
    return _devices;
}

void QZeroPropsClient::startDiscovery(const Configuration& config)
{
    m_services.clear();
    emit servicesChanged();

    if (!config.zeroConfType.isEmpty()) {
        m_zeroConf->startBrowser(config.zeroConfType);
    }

    if (config.bleUuid.isNull()) {
        // m_ble...
    }
}

void QZeroPropsClient::stopDiscovery()
{
    m_zeroConf->stopBrowser();
}

void QZeroPropsClient::connectToService(QZeroPropsService* service)
{
    disconnectFromService();

    if (!service) {
        return;
    }

    m_currentService = service;
    connect(m_currentService, &QZeroPropsService::stateChanged, this, &QZeroPropsClient::onStatus);
    m_currentService->d->connect();
}

void QZeroPropsClient::disconnectFromService()
{
    if (m_currentService) {
        m_currentService->d->disconnect();
        m_currentService->disconnect();
        m_currentService = nullptr;
    }
}

void QZeroPropsClient::onServiceDiscovered(QZeroConfService service)
{
    QZeroPropsServicePtr device(new QZeroPropsService);
    device->m_name = service->name();
    device->m_type = QZeroPropsService::Type::WebSocket;
    device->m_address = service->ip();
    device->m_port = service->port();

    m_services.push_back(device);
    emit servicesChanged();
}

void QZeroPropsClient::onServiceRemoved(QZeroConfService service)
{
    for (auto it = m_services.begin(); it != m_services.end(); ++it) {
        if ((*it)->m_address == service->ip()) {
            it = m_services.erase(it);
            if (it == m_services.end()) {
                break;
            }
        }
    }

    emit servicesChanged();
}

void QZeroPropsClient::onStatus(QZeroPropsClient::State _status, QString errorString)
{
    emit stateChanged(_status, errorString);
}

} // namespace QZeroProps
