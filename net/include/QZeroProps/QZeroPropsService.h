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

#include <QZeroProps/QZeroPropsClient.h>

#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtNetwork/QHostAddress>

namespace QZeroProps
{
class QZeroPropsServicePrivate;

class QZeroPropsService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(ServiceType type READ type)

public:
    // We cannot use an alias here
    //using DeviceType = common::CtrlInterfaceType;
    enum class ServiceType {
        Invalid = 0,
        BluetoothLe = 0x1,
        WebSocket = 0x2,
        All = BluetoothLe | WebSocket
    };
    Q_ENUM(ServiceType)

    virtual ~QZeroPropsService();

    QString name() const;
    ServiceType type() const;

public slots:
    void setDebounceTime(int msec);

    // Property setters with explicit id types instead of variant. BluetoothLE only supports few types as id.
    void setProperty(quint32 id, const QByteArray& value);
    void setProperty(const QUuid& uuid, const QByteArray& value);

signals:
    void stateChanged(QZeroPropsClient::State state, const QString& errorString = QString());
    void propertyChanged(const QVariant& uuid, const QByteArray& value);

protected:
    void connect();

private:
    QZeroPropsService(QObject* parent = nullptr);

    // make non-const to later add custom impl
    class QZeroPropsServicePrivate* d;

    friend class QZeroPropsClient;
    friend class QZeroPropsClientPrivate;
    friend class QZeroPropsServicePrivate;
};

using QZeroPropsServicePtr = QSharedPointer<QZeroPropsService>;

} // namespace QZeroProps
