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

#include <QHostAddress>
#include <QTimer>

#include <common/Types.h>

class QZeroConf;
class QZeroConfServiceData;
typedef QSharedPointer<QZeroConfServiceData> QZeroConfService;

namespace net
{

struct NetDevice
{
public:
    using Type = common::CtrlInterfaceType;

    Q_GADGET

    Q_ENUM(Type)

    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(Type type MEMBER type)

public:
    QString name;
    Type type = Type::Invalid;

    QHostAddress address;
    uint16_t port = 0;
};

class NetClient : public QObject
{
    Q_OBJECT

public:
    enum class Status
    {
        NoBluetooth,    // Currently unused, since we cannot tell on iOS wether Bluetooth is on.
        Discovering,
        Connecting,     // Connecting state hangs from time to time, so provide user interaction
        Connected,
        Timeout,
        Lost,
        Error
    };

    explicit NetClient(QObject *parent = nullptr);
    ~NetClient();

    bool startDiscovering();
	void stopDiscovering();

signals:
    void status(Status status, const QString& errorString = QString());
    void deviceDiscovered(const NetDevice& device);

private:
    void onServiceAdded(QZeroConfService);

    QZeroConf* m_zeroConf = nullptr;
};

} // namespace net
