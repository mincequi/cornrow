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
#include <QUuid>
#include <QtEndian>
#include <QtNetwork/QTcpSocket>
#include <QtWebSockets/QWebSocket>

#include <qzeroconf.h>

#include <loguru/loguru.hpp>

#include <cmath>
#include <unistd.h>

using namespace std::placeholders;

namespace net
{

TcpServer::TcpServer(QObject *parent)
    : QObject(parent),
      m_server("", QWebSocketServer::SslMode::NonSecureMode)
{
    // Open service
    connect(&m_server, &QWebSocketServer::newConnection, this, &TcpServer::onClientConnected);
    if (!m_server.listen(QHostAddress::AnyIPv4)) {
        LOG_F(ERROR, "Error starting NetService.");
        return;
    }

    startPublishing();
}

TcpServer::~TcpServer()
{
}

void TcpServer::startPublishing()
{
    // Publish service
    QZeroConf* zeroConf = new QZeroConf(this);
    connect(zeroConf, &QZeroConf::servicePublished, [&]() {
        LOG_F(INFO, "TCP server published at port: %i", m_server.serverPort());
    });
    connect(zeroConf, &QZeroConf::error, [](QZeroConf::error_t error) {
        LOG_F(WARNING, "Error publishing service: %i", error);
    });
    zeroConf->startServicePublish(QHostInfo::localHostName().toStdString().c_str(),
                                  "_cornrow._tcp",
                                  nullptr,
                                  m_server.serverPort());
}

void TcpServer::disconnect()
{
    if (!m_client) {
        return;
    }
    m_client->abort();
    m_client->deleteLater();
    m_client = nullptr;
}

void TcpServer::setProperty(const QUuid& name, const QByteArray& value)
{
    auto _name = name.toByteArray(QUuid::WithoutBraces);
    QObject::setProperty(_name, value);

    //doSend(_name);
}

void TcpServer::onClientConnected()
{
    if (m_client) {
        LOG_F(INFO, "Another client already connected");
        return;
    }

    // Open socket
    m_client = m_server.nextPendingConnection();

    connect(m_client, &QWebSocket::binaryMessageReceived, this, &TcpServer::onReceive);
    connect(m_client, &QWebSocket::disconnected, this, &TcpServer::disconnect);

    // Serialize dynamic properties
    LOG_F(INFO, "New connection. Send properties:");
    QByteArray block;
    QDataStream outStream(&block, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_5_6);
    QVariantMap properties;
    for (const auto& name : dynamicPropertyNames()) {
        const auto value = property(name).toByteArray();
        LOG_F(INFO, "%s: %i", name.toStdString().c_str(), value.size());
        properties.insert(name, value);
    }
    outStream << properties;

    // Send buffer
    const auto bytes = m_client->sendBinaryMessage(block);
    LOG_F(INFO, "Wrote %i bytes", bytes);
    m_client->flush();
}

void TcpServer::onReceive(const QByteArray& message)
{
    QDataStream dataStream(message);
    dataStream.setVersion(QDataStream::Qt_5_6);

    // Deserialize data
    dataStream.startTransaction();
    QVariantMap properties;
    dataStream >> properties;
    if (!dataStream.commitTransaction()) {
        qWarning("Error deserializing data");
        return;
    }

    for (const auto& kv : properties.toStdMap()) {
        const char* name = kv.first.toLatin1().constData();
        const QByteArray value = kv.second.toByteArray();
        QObject::setProperty(name, value);
        emit propertyChanged(name, value);
    }
}

void TcpServer::doSend(const QByteArray& name)
{
    // If we are not connected, we do not send
    if (!m_client || m_client->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Socket not connected";
        return;
    }

    // Send property
    QVariantMap properties;
    const auto value = property(name).toByteArray();
    qDebug() << "Send property:" << name << ", value size:" << value.size();
    properties.insert(name, value);

    // Write out
    QByteArray block;
    QDataStream outStream(&block, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_5_6);
    outStream << properties;
    const auto bytes = m_client->sendBinaryMessage(block);
    qDebug() << "Wrote" << bytes << "bytes";
    m_client->flush();
}

} // namespace net
