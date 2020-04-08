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

#include "QZeroPropsWsService.h"

#include <msgpack.h>

#include <QUuid>

namespace QtZeroProps
{

QZeroPropsWsService::QZeroPropsWsService(QZeroPropsService* _q)
    : QZeroPropsServicePrivate(_q)
{
    MsgPack::registerPacker(QVariant::Type::Uuid, QVariant::Type::Uuid, [](const QVariant& variant) -> QByteArray {
        return variant.toUuid().toRfc4122();
    });
    MsgPack::registerUnpacker(QVariant::Type::Uuid, [](const QByteArray& buffer) -> QVariant {
        return QUuid::fromRfc4122(buffer);
    });
}

QZeroPropsWsService::~QZeroPropsWsService()
{
    disconnect();
}

void QZeroPropsWsService::connect()
{
    emit stateChanged(QZeroPropsClient::State::Connecting, "Connecting " + q->name());

    if (socket) {
        socket->disconnect();
        socket->deleteLater();
    }

    socket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);

    // Setup socket
    QObject::connect(socket, &QWebSocket::binaryMessageReceived, [this](const QByteArray &message) {
        onReceive(message);
    });
    QObject::connect(socket, &QWebSocket::connected, [this]() {
        emit stateChanged(QZeroPropsClient::State::Connected);
    });
    QObject::connect(socket, &QWebSocket::disconnected, [this]() {
        emit stateChanged(QZeroPropsClient::State::Disconnected);
    });
    QObject::connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), [this](QAbstractSocket::SocketError) {
        emit stateChanged(QZeroPropsClient::State::Error, socket->errorString());
    });
    QObject::connect(socket, &QWebSocket::pong, [](quint64 elapsedTime, const QByteArray &payload) {
        qDebug() << "pong> elapsedTime:" << elapsedTime << ", payload.size: " << payload.size();
    });
    QObject::connect(socket, &QWebSocket::stateChanged, [this](QAbstractSocket::SocketState state) {
        onStateChanged(state);
    });

    QUrl url;
    url.setScheme("ws");
    url.setHost(address.toString());
    url.setPort(port);
    socket->open(url);
}

void QZeroPropsWsService::disconnect()
{
    if (socket) {
        socket->disconnect();
        socket->abort();
        socket->deleteLater();
        socket = nullptr;
    }
}

void QZeroPropsWsService::onClientConnected(QWebSocket* _socket)
{
    if (socket) {
        qDebug("Another client already connected");
        return;
    }

    // Open socket
    socket = _socket;

    QObject::connect(socket, &QWebSocket::binaryMessageReceived, this, &QZeroPropsWsService::onReceive);
    QObject::connect(socket, &QWebSocket::disconnected, this, &QZeroPropsWsService::disconnect);

    // Iterate dirty properties and send them
    qDebug("New connection. Send properties:");
    for (const auto& kv : properties.toStdMap()) {
        const auto& key = kv.first;
        const auto& value = kv.second;
        qDebug("%s: %i", key.toByteArray().toStdString().c_str(), value.size());

        QByteArray block;
        block += static_cast<char>(0x81);   // Map with one element
        block += MsgPack::pack(key);
        block += MsgPack::pack(value);
        const auto bytes = socket->sendBinaryMessage(block);
        qDebug("Wrote %lld bytes", bytes);
    }
    socket->flush();
}

void QZeroPropsWsService::onStateChanged(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::UnconnectedState:
        break;
    case QAbstractSocket::HostLookupState:
    case QAbstractSocket::ConnectingState:
        emit stateChanged(QZeroPropsClient::State::Connecting);
        break;
    case QAbstractSocket::ConnectedState:
        emit stateChanged(QZeroPropsClient::State::Connected);
        break;
    case QAbstractSocket::BoundState:
    case QAbstractSocket::ClosingState:
        break;
    default:
        break;
    }
}

void QZeroPropsWsService::onReceive(const QByteArray& message)
{
    if (message.front() != static_cast<char>(0x81)) {
        qWarning() << "Illegal data:" << message.front();
        return;
    }

    QVariantList vlist = MsgPack::unpack(message.mid(1)).toList();
    if (vlist.size() != 2 ||
            (vlist.at(0).type() != QVariant::Type::UInt && vlist.at(0).type() != QVariant::Type::Uuid) ||
            vlist.at(1).type() != QVariant::Type::ByteArray) {
        qWarning() << "Illegal data:" << vlist;
        return;
    }

    auto value = vlist.last().toByteArray();

    properties.insert(vlist.first(), value);
    emit q->propertyChanged(vlist.first(), value);
}

void QZeroPropsWsService::doSend(const QVariant& uuid, const QByteArray& value)
{
    // If we are not connected, we do not send
    if (!socket || socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Socket not connected";
        return;
    }

    QByteArray block;
    block += static_cast<char>(0x81);   // Map with one element
    block += MsgPack::pack(uuid);
    block += MsgPack::pack(value);
    const auto bytes = socket->sendBinaryMessage(block);
    qDebug() << "Sent" << bytes << "bytes";

    socket->flush();
}

} // namespace QZeroProps
