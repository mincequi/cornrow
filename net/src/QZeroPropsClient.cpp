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

#include "QZeroPropsClient.h"

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
    : QObject(parent)
{
    // Setup zeroconf
    m_zeroConf = new QZeroConf(this);
    connect(m_zeroConf, &QZeroConf::serviceAdded, this, &QZeroPropsClient::onServiceDiscovered);
    connect(m_zeroConf, &QZeroConf::serviceRemoved, this, &QZeroPropsClient::onServiceRemoved);
}

QZeroPropsClient::~QZeroPropsClient()
{
}

void QZeroPropsClient::startDiscovery(const Configuration& config)
{
    Q_UNUSED(config)
    m_zeroConf->startBrowser("_cornrow._tcp");
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

    emit deviceDiscovered(device);
    qDebug() << __func__ << ">" << service;
}

void QZeroPropsClient::onServiceRemoved(QZeroConfService service)
{
    emit deviceDisappeared(service->ip());
}

void QZeroPropsClient::onStatus(QZeroPropsClient::State _status, QString errorString)
{
    emit stateChanged(_status, errorString);
}

} // namespace QZeroProps
