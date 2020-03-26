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
#include <QtGlobal>
#include <qzeroconf.h>

#include <common/RemoteDataStore.h>

namespace net
{

TcpClient::TcpClient(common::RemoteDataStore* remoteStore, QObject* parent)
    : QObject(parent),
      m_remoteStore(remoteStore)
{
    m_dataStream.setDevice(&m_socket);
    m_dataStream.setVersion(QDataStream::Qt_5_6);

    m_zeroConf = new QZeroConf(this);
    connect(m_zeroConf, &QZeroConf::serviceAdded, this, &TcpClient::onServiceAdded);

    connect(&m_socket, &QTcpSocket::connected, [this]() {
        onStatus(ble::BleClient::Status::Connected);
    });
    connect(&m_socket, &QTcpSocket::disconnected, [this]() {
        onStatus(ble::BleClient::Status::Lost);
    });
    connect(&m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), [this](QAbstractSocket::SocketError) {
        onStatus(ble::BleClient::Status::Error, m_socket.errorString());
    });
    connect(&m_socket, &QIODevice::readyRead, this, &TcpClient::onDataReceived);
}

TcpClient::~TcpClient()
{
}

void TcpClient::startDiscovering()
{
    if (m_zeroConf->browserExists()) {
        stopDiscovering();
    }

    m_zeroConf->startBrowser("_cornrow._tcp");
}

void TcpClient::stopDiscovering()
{
    m_zeroConf->stopBrowser();
}

void TcpClient::connectDevice(net::NetDevice* device)
{
    disconnect();

    m_socket.connectToHost(device->address, device->port);
}

void TcpClient::disconnect()
{
    m_socket.abort();
}

void TcpClient::setProperty(const std::string& name, const QByteArray& value)
{
    if (m_socket.state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Socket not connected";
        return;
    }

    QByteArray block;
    QDataStream dataStream(&block, QIODevice::WriteOnly);
    dataStream.setVersion(QDataStream::Qt_5_6);
    dataStream << QVariantMap( {{ name.c_str(), value }} );

    m_socket.write(block);
}

void TcpClient::onServiceAdded(QZeroConfService service)
{
    NetDevicePtr device(new NetDevice);
    device->name = service->name();
    device->type = NetDevice::DeviceType::TcpIp;
    device->address = service->ip();
    device->port = service->port();

    emit deviceDiscovered(device);
    qDebug() << service;
}

void TcpClient::onStatus(ble::BleClient::Status _status, QString)
{
    switch (_status) {
    case ble::BleClient::Status::Connected: {
        /*
        m_remote.addClientSideConnection(&m_socket);
        m_replica = m_remote.acquireDynamic("CornrowData");
        qDebug() << "Replica initialized: " << m_replica->isInitialized();
        connect(m_replica, &QRemoteObjectDynamicReplica::initialized, [&]() {
            for (int i = m_replica->metaObject()->propertyOffset(); i < m_replica->metaObject()->propertyCount(); ++i) {
                qDebug() << "Property: " << m_replica->metaObject()->property(i).name();
            }
        });
        connect(m_replica, &QRemoteObjectDynamicReplica::stateChanged, [&]() {
            qDebug() << "Cornrow replica state: " << m_replica->state();
        });
        */
    }
        break;
    default:
        break;
    }

    emit status(_status);
}

void TcpClient::onDataReceived()
{
    m_dataStream.startTransaction();

    QVariantMap properties;
    m_dataStream >> properties;

    if (!m_dataStream.commitTransaction()) {
        qWarning("Error deserializing data");
        return;
    }

    for (const auto& kv : properties.toStdMap()) {
        m_remoteStore->setProperty(kv.first.toStdString().c_str(), kv.second);
    }
}

} // namespace net
