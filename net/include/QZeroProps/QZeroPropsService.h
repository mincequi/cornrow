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

#include <QBluetoothDeviceInfo>
#include <QHostAddress>
#include <QSet>
#include <QTimer>
#include <QUuid>
#include <QVariant>

#include <QZeroProps/QZeroPropsClient.h>

namespace QZeroProps
{
class QZeroPropsServicePrivate;

class QZeroPropsService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name)
    Q_PROPERTY(Type type MEMBER m_type)

public:
    // We cannot use an alias here
    //using DeviceType = common::CtrlInterfaceType;
    enum class Type {
        Invalid = 0,
        BluetoothLe = 0x1,
        WebSocket = 0x2,
        All = BluetoothLe | WebSocket
    };
    // We need this for QML engine
    Q_ENUM(Type)

    QZeroPropsService(QZeroPropsClient* parent = nullptr);
    virtual ~QZeroPropsService();

public slots:
    void setDebounceTime(int msec);

    // Property setters with explicit id types instead of variant. BluetoothLE only supports few types as id.
    void setProperty(quint32 id, const QByteArray& value);
    void setProperty(const QUuid& uuid, const QByteArray& value);

signals:
    void stateChanged(QZeroPropsClient::State state, const QString& errorString = QString());
    void propertyChanged(const QVariant& uuid, const QByteArray& value);

private:
    class QZeroPropsServicePrivate* const d;

public:
    // @TODO(mawe): this needs to be reworked
    QString m_name;
    Type m_type = Type::Invalid;

    QHostAddress m_address;
    uint16_t m_port = 0;

    QBluetoothDeviceInfo m_bluetoothDeviceInfo;

    friend class QZeroPropsClient;
};

using QZeroPropsServicePtr = QSharedPointer<QZeroPropsService>;

} // namespace QZeroProps

Q_DECLARE_METATYPE(QZeroProps::QZeroPropsServicePtr)
