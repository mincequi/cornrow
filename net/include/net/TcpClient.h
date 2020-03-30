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

#pragma once

// @TODO(mawe): remove bluetooth dependency
#include <QBluetoothDeviceInfo>
#include <QDataStream>
#include <QHostAddress>
#include <QTcpSocket>
#include <QTimer>

#include <ble/BleClient.h>
#include <common/Types.h>

class QTcpSocket;
class QZeroConf;
class QZeroConfServiceData;
typedef QSharedPointer<QZeroConfServiceData> QZeroConfService;

namespace common {
class RemoteDataStore;
}

namespace net
{

struct NetDevice : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(DeviceType type MEMBER type)

public:
	// We cannot use an alias here
    //using DeviceType = common::CtrlInterfaceType;
    enum class DeviceType {
		Invalid = 0,
		BluetoothLe = 0x1,
		TcpIp = 0x2
    };
    // We need this for QML engine
    Q_ENUM(DeviceType)
    
    QString name;
    DeviceType type = DeviceType::Invalid;

    QHostAddress address;
    uint16_t port = 0;
    
    QBluetoothDeviceInfo bluetoothDeviceInfo;
};
using NetDevicePtr = QSharedPointer<net::NetDevice>;

class TcpClient : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QObject *parent = nullptr);
    ~TcpClient();

    void startDiscovering();
	void stopDiscovering();

    void connectDevice(net::NetDevice* device);
    void disconnect();

    void setProperty(const QUuid& uuid, const QByteArray& value);

signals:
    void status(ble::BleClient::Status status, const QString& errorString = QString());
    void deviceDiscovered(NetDevicePtr device);
    void deviceDisappeared(const QHostAddress& ip);
    void propertyChanged(const QUuid& uuid, const QByteArray& value);

private:
    // Device related event handlers
    void onServiceDiscovered(QZeroConfService);
    void onServiceRemoved(QZeroConfService);
    void onStatus(ble::BleClient::Status status, QString errorString = QString());

    // Property related event handlers / action
    void onReceive();
    void doSend();

    QZeroConf* m_zeroConf = nullptr;

    QTcpSocket  m_socket;
    QDataStream m_dataStream;

    QTimer              m_timer;
    QSet<QByteArray>    m_dirtyProperties;

};

} // namespace net

Q_DECLARE_METATYPE(net::NetDevicePtr)
