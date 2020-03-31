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

#include "TcpClient.h"

#include <QUuid>

#include <msgpack.h>
#include <qzeroconf.h>

namespace net
{

TcpClient::TcpClient(QObject* parent)
    : QObject(parent)
{
    // Setup timer
    m_timer.setInterval(200);
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, &TcpClient::doSend);

    // Setup socket
    connect(&m_socket, &QWebSocket::binaryMessageReceived, [this](const QByteArray &message) {
        onReceive(message);
    });
    connect(&m_socket, &QWebSocket::connected, []() {
        qDebug() << "connected";
    });
    connect(&m_socket, &QWebSocket::disconnected, [this]() {
        onStatus(ble::BleClient::Status::Lost);
    });
    connect(&m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), [this](QAbstractSocket::SocketError error) {
        qDebug() << "error:" << error;
        onStatus(ble::BleClient::Status::Error, m_socket.errorString());
    });
    connect(&m_socket, &QWebSocket::pong, [](quint64 elapsedTime, const QByteArray &payload) {
        qDebug() << "pong> elapsedTime:" << elapsedTime << ", payload.size: " << payload.size();
    });
    connect(&m_socket, &QWebSocket::stateChanged, [](QAbstractSocket::SocketState state) {
        qDebug() << "stateChanged:" << state;
    });

    // Setup zeroconf
    m_zeroConf = new QZeroConf(this);
    connect(m_zeroConf, &QZeroConf::serviceAdded, this, &TcpClient::onServiceDiscovered);
    connect(m_zeroConf, &QZeroConf::serviceRemoved, this, &TcpClient::onServiceRemoved);

    MsgPack::registerPacker(QVariant::Type::Uuid, QVariant::Type::Uuid, [](const QVariant& variant) -> QByteArray {
        return variant.toUuid().toRfc4122();
    });
    MsgPack::registerUnpacker(QVariant::Type::Uuid, [](const QByteArray& buffer) -> QVariant {
        return QUuid::fromRfc4122(buffer);
    });
}

TcpClient::~TcpClient()
{
}

void TcpClient::startDiscovering()
{
    m_zeroConf->startBrowser("_cornrow._tcp");
}

void TcpClient::stopDiscovering()
{
    m_zeroConf->stopBrowser();
}

void TcpClient::connectDevice(net::NetDevice* device)
{
    disconnect();

    QUrl url;
    url.setScheme("ws");
    url.setHost(device->address.toString());
    url.setPort(device->port);
    m_socket.open(url);
}

void TcpClient::disconnect()
{
    m_socket.abort();
}

void TcpClient::setDebounceTime(int msec)
{
    m_timer.setInterval(msec);
}

/*
void TcpClient::setProperty(quint32 uuid, const QByteArray& value)
{
    QByteArray store;
    store << uuid;
    m_properties.insert(uuid, value);

    m_dirtyProperties.insert(uuid);
    if (!m_timer.isActive()) {
        m_timer.start();
    }
}
*/

void TcpClient::setProperty(const QUuid& uuid, const QByteArray& value)
{
    m_properties.insert(uuid, value);

    auto _name = uuid.toByteArray(QUuid::WithoutBraces);
    QObject::setProperty(_name, value);

    m_dirtyProperties.insert(uuid);
    if (!m_timer.isActive()) {
        m_timer.start();
    }
}

void TcpClient::onServiceDiscovered(QZeroConfService service)
{
    NetDevicePtr device(new NetDevice);
    device->name = service->name();
    device->type = NetDevice::DeviceType::TcpIp;
    device->address = service->ip();
    device->port = service->port();

    emit deviceDiscovered(device);
    qDebug() << __func__ << ">" << service;
}

void TcpClient::onServiceRemoved(QZeroConfService service)
{
    emit deviceDisappeared(service->ip());
}

void TcpClient::onStatus(ble::BleClient::Status _status, QString)
{
    emit status(_status);
}

void TcpClient::doSend()
{
    // If we are not connected, we do not send
    if (m_socket.state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Socket not connected";
        return;
    }

    // Iterate dirty properties and send them
    for (const auto& key : m_dirtyProperties) {
        const auto& value = m_properties[key];
        qDebug() << "Send property:" << key << ", value size:" << value.size();

        QByteArray block;
        block += static_cast<char>(0x81);   // Map with one element
        block += MsgPack::pack(key);
        block += MsgPack::pack(value);
        const auto bytes = m_socket.sendBinaryMessage(block);
        qDebug() << "Wrote" << bytes << "bytes";
    }
    m_socket.flush();

    // Clear dirty propertiers
    m_dirtyProperties.clear();
}

void TcpClient::onReceive(const QByteArray& message)
{
    if (message.front() != static_cast<char>(0x81)) {
        qDebug() << "Illegal data:" << message.front();
        return;
    }

    auto key = MsgPack::unpack(message.mid(1, 18)).toUuid();
    auto value = MsgPack::unpack(message.mid(19)).toByteArray();
    qDebug("uuid: %s, value size: %i", key.toByteArray().constData(), value.size());
    m_properties.insert(key, value);
    emit propertyChanged(key, value);

    onStatus(ble::BleClient::Status::Connected);
}

} // namespace net
