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

#include "QZeroPropsWsServer.h"

#include "QZeroPropsWsService.h"

#include <QDebug>
#include <QHostInfo>
#include <QtWebSockets/QWebSocket>

#include <qzeroconf.h>

namespace QtZeroProps
{

QZeroPropsWsServer::QZeroPropsWsServer(QObject *parent)
    : QtZeroProps::QZeroPropsServerPrivate(parent),
      m_server("", QWebSocketServer::SslMode::NonSecureMode)
{
}

QZeroPropsWsServer::~QZeroPropsWsServer()
{
}

QZeroPropsServicePrivate* QZeroPropsWsServer::createService(const QtZeroProps::ServiceConfiguration& config)
{
    // Open socket
    m_server.close();
    if (!m_server.listen(QHostAddress::AnyIPv4)) {
        qWarning("Error starting NetService.");
        return nullptr;
    }

    m_service = new QZeroPropsWsService(m_currentService);
    m_service->name = config.zeroConfType;
    m_service->type = QZeroPropsService::ServiceType::WebSocket;
    connect(&m_server, &QWebSocketServer::newConnection, [this]() {
        m_service->onClientConnected(m_server.nextPendingConnection());
    });

    // Publish service
    QZeroConf* zeroConf = new QZeroConf(this);
    connect(zeroConf, &QZeroConf::servicePublished, [&]() {
        qDebug("TCP server published at port: %i", m_server.serverPort());
    });
    connect(zeroConf, &QZeroConf::error, [](QZeroConf::error_t error) {
        qWarning("Error publishing service: %i", error);
    });
    zeroConf->startServicePublish(QHostInfo::localHostName().toStdString().c_str(),
                                  config.zeroConfType.toStdString().c_str(),
                                  nullptr,
                                  m_server.serverPort());

    return m_service;
}

} // namespace net
