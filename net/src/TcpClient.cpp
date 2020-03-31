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

#include <QDebug>
#include <QTcpSocket>
#include <QUuid>
#include <QtGlobal>
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

    /*
    //connect(&m_socket, &QTcpSocket::connected, [this]() {
    //    onStatus(ble::BleClient::Status::Connected);
    //});
    connect(&m_socket, &QTcpSocket::stateChanged, [](QAbstractSocket::SocketState state) {
        qDebug() << "stateChanged:" << state;
    });
    connect(&m_socket, &QTcpSocket::disconnected, [this]() {
        onStatus(ble::BleClient::Status::Lost);
    });
    connect(&m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), [this](QAbstractSocket::SocketError) {
        onStatus(ble::BleClient::Status::Error, m_socket.errorString());
    });
    connect(&m_socket, &QIODevice::readyRead, this, &TcpClient::onReceive);
    */
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

void TcpClient::setProperty(const QUuid& name, const QByteArray& value)
{
    auto _name = name.toByteArray(QUuid::WithoutBraces);
    QObject::setProperty(_name, value);

    m_dirtyProperties.insert(_name);
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

    // Iterate dirty properties and serialize them
    QVariantMap properties;
    for (const auto& name : m_dirtyProperties) {
        const auto value = property(name).toByteArray();
        qDebug() << "Send property:" << name << ", value size:" << value.size();
        properties.insert(name, value);
    }

    // Write out
    QByteArray block;
    QDataStream outStream(&block, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_5_6);
    outStream << properties;
    const auto bytes = m_socket.sendBinaryMessage(block);
    qDebug() << "Wrote" << bytes << "bytes";
    m_socket.flush();

    // Clear dirty propertiers
    m_dirtyProperties.clear();
}

void TcpClient::onReceive(const QByteArray& message)
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

    // Apply data and emit changes
    qDebug("Data received:");
    for (const auto& kv : properties.toStdMap()) {
        const auto key = kv.first.toLatin1();
        const auto val = kv.second.toByteArray();
        qDebug("name: %s, value size: %i", key.constData(), val.size());
        QObject::setProperty(key.constData(), val);
        emit propertyChanged(key, val);
    }

    onStatus(ble::BleClient::Status::Connected);
}

} // namespace net
