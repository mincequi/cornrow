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
#include <QTimer>
#include <QtWebSockets/QWebSocket>

#include <ble/BleClient.h>

class QZeroConf;
class QZeroConfServiceData;
typedef QSharedPointer<QZeroConfServiceData> QZeroConfService;

namespace net
{

uint qHash(const QVariant& var);

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

    Q_INVOKABLE void startDiscovering();
    Q_INVOKABLE void stopDiscovering();

    Q_INVOKABLE void connectDevice(net::NetDevice* device);
    Q_INVOKABLE void disconnect();

    Q_INVOKABLE void setDebounceTime(int msec);

    // @TODO(mawe): implement uint based properties (need to store as a key together with Uuids somehow).
    //void setProperty(quint32 uuid, const QByteArray& value);
    void setProperty(const QUuid& uuid, const QByteArray& value);

signals:
    void status(ble::BleClient::Status status, const QString& errorString = QString());
    void deviceDiscovered(NetDevicePtr device);
    void deviceDisappeared(const QHostAddress& ip);
    void propertyChanged(const QUuid& uuid, const QByteArray& value);

private:
    static QByteArray toMsgPack(const QVariant& key, const QByteArray& value);

    // Device related event handlers
    void onServiceDiscovered(QZeroConfService);
    void onServiceRemoved(QZeroConfService);
    void onStatus(ble::BleClient::Status status, QString errorString = QString());

    // Property related event handlers / action
    void onReceive(const QByteArray &message);
    void doSend();

    QZeroConf* m_zeroConf = nullptr;

    QWebSocket  m_socket;

    QTimer      m_timer;
    QSet<QUuid> m_dirtyProperties;
    QMap<QUuid, QByteArray>  m_properties;
};

} // namespace net

Q_DECLARE_METATYPE(net::NetDevicePtr)
