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

#include <QCoreApplication>
#include <QHostInfo>
#include <QUuid>
#include <QtWebSockets/QWebSocket>

#include <msgpack.h>
#include <qzeroconf.h>
#include <loguru/loguru.hpp>

using namespace std::placeholders;

namespace net
{

QZeroPropsWsServer::QZeroPropsWsServer(QObject *parent)
    : QObject(parent),
      m_server("", QWebSocketServer::SslMode::NonSecureMode)
{
    // Open service
    connect(&m_server, &QWebSocketServer::newConnection, this, &QZeroPropsWsServer::onClientConnected);
    if (!m_server.listen(QHostAddress::AnyIPv4)) {
        LOG_F(ERROR, "Error starting NetService.");
        return;
    }

    startPublishing();

    MsgPack::registerPacker(QVariant::Type::Uuid, QVariant::Type::Uuid, [](const QVariant& variant) -> QByteArray {
        return variant.toUuid().toRfc4122();
    });
    MsgPack::registerUnpacker(QVariant::Type::Uuid, [](const QByteArray& buffer) -> QVariant {
        return QUuid::fromRfc4122(buffer);
    });
}

QZeroPropsWsServer::~QZeroPropsWsServer()
{
}

void QZeroPropsWsServer::startPublishing()
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

void QZeroPropsWsServer::disconnect()
{
    if (!m_client) {
        return;
    }
    m_client->abort();
    m_client->deleteLater();
    m_client = nullptr;
}

void QZeroPropsWsServer::setProperty(const QUuid& uuid, const QByteArray& value)
{
    m_properties.insert(uuid, value);

    //doSend(_name);
}

void QZeroPropsWsServer::onClientConnected()
{
    if (m_client) {
        LOG_F(INFO, "Another client already connected");
        return;
    }

    // Open socket
    m_client = m_server.nextPendingConnection();

    connect(m_client, &QWebSocket::binaryMessageReceived, this, &QZeroPropsWsServer::onReceive);
    connect(m_client, &QWebSocket::disconnected, this, &QZeroPropsWsServer::disconnect);

    // Iterate dirty properties and send them
    LOG_F(INFO, "New connection. Send properties:");
    for (const auto& kv : m_properties.toStdMap()) {
        const auto& key = kv.first;
        const auto& value = kv.second;
        LOG_F(INFO, "%s: %i", key.toByteArray().toStdString().c_str(), value.size());

        QByteArray block;
        block += static_cast<char>(0x81);   // Map with one element
        block += MsgPack::pack(key);
        block += MsgPack::pack(value);
        const auto bytes = m_client->sendBinaryMessage(block);
        LOG_F(INFO, "Wrote %lld bytes", bytes);
    }
    m_client->flush();
}

void QZeroPropsWsServer::onReceive(const QByteArray& message)
{
    if (message.front() != static_cast<char>(0x81)) {
        qDebug() << "Illegal data:" << message.front();
        return;
    }

    auto key = MsgPack::unpack(message.mid(1, 18)).toUuid();
    auto value = MsgPack::unpack(message.mid(19)).toByteArray();
    LOG_F(INFO, "uuid: %s, value size: %i", key.toByteArray().constData(), value.size());
    m_properties.insert(key, value);
    emit propertyChanged(key, value);
}

} // namespace net
