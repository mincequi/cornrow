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

#include "TcpServer.h"

#include <QAbstractSocket>
#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>
#include <QHostInfo>
#include <QMetaProperty>
#include <QThread>
#include <QTimer>
#include <QtEndian>
#include <QtNetwork/QTcpSocket>

#include <qzeroconf.h>

#include <common/RemoteDataStore.h>
#include <loguru/loguru.hpp>

#include <cmath>
#include <unistd.h>

using namespace std::placeholders;

namespace net
{

TcpServer::TcpServer(common::RemoteDataStore* remoteStore, QObject *parent)
    : QObject(parent),
      m_remoteData(remoteStore)
{
    m_inStream.setVersion(QDataStream::Qt_5_6);

    // Open service
    connect(&m_tcpServer, &QTcpServer::newConnection, this, &TcpServer::onClientConnected);
    if (!m_tcpServer.listen(QHostAddress::AnyIPv4)) {
        LOG_F(ERROR, "Error starting NetService.");
        return;
    }

    // Publish service
    QZeroConf* zeroConf = new QZeroConf(this);
    connect(zeroConf, &QZeroConf::servicePublished, [&]() {
        LOG_F(INFO, "TCP server published at port: %i", m_tcpServer.serverPort());
    });
    connect(zeroConf, &QZeroConf::error, [](QZeroConf::error_t error) {
        LOG_F(WARNING, "Error publishing service: %i", error);
    });
    zeroConf->startServicePublish(QHostInfo::localHostName().toStdString().c_str(),
                                  "_cornrow._tcp",
                                  nullptr,
                                  m_tcpServer.serverPort());
}

TcpServer::~TcpServer()
{
}

void TcpServer::disconnect()
{
    if (!m_socket) {
        return;
    }
    m_socket->abort();
    m_socket->deleteLater();
    m_socket = nullptr;
}

void TcpServer::onClientConnected()
{
    if (m_socket) {
        LOG_F(INFO, "Another client already connected");
        return;
    }

    QVariantMap properties;
    LOG_F(INFO, "New connection. Send properties:");
    for (int i = m_remoteData->metaObject()->propertyOffset(); i < m_remoteData->metaObject()->propertyCount(); ++i) {
        const char* name = m_remoteData->metaObject()->property(i).name();
        const QVariant value = m_remoteData->metaObject()->property(i).read(m_remoteData);
        LOG_F(INFO, "%s: %s", name, value);
        properties.insert(name, value);
    }

    m_socket = m_tcpServer.nextPendingConnection();
    connect(m_socket, &QAbstractSocket::disconnected, this, &TcpServer::disconnect);
    connect(m_socket, &QAbstractSocket::readyRead, this, &TcpServer::onDataReceived);
    m_inStream.setDevice(m_socket);

    QByteArray block;
    QDataStream outStream(&block, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_5_6);
    outStream << properties;
    const auto bytes = m_socket->write(block);
    LOG_F(INFO, "Wrote %i bytes", bytes);
    m_socket->flush();
}

void TcpServer::onDataReceived()
{
    m_inStream.startTransaction();

    QVariantMap properties;
    m_inStream >> properties;

    if (!m_inStream.commitTransaction()) {
        LOG_F(WARNING, "Error deserializing data");
        return;
    }

    for (const auto& kv : properties.toStdMap()) {
        m_remoteData->setProperty(kv.first.toStdString().c_str(), kv.second);
    }
}

} // namespace net
