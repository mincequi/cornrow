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

#include <QZeroProps/QZeroPropsTypes.h>

namespace QZeroProps
{

class QZeroPropsClient : public QObject
{
    Q_OBJECT

    // @TODO(Qt): QObjectList is not accepted by QmlEngine. So, we must use QList<QObject*>.
    Q_PROPERTY(QList<QObject*> discoveredServices READ discoveredServices NOTIFY servicesChanged)

public:
    enum State : uint8_t {
        Idle,           // Shall be triggered from discoverer
        Discovering,    // Shall be triggered from discoverer
        Connecting,
        Connected,
        Disconnected,
        Error
    };
    Q_ENUM(State)

    /*
    enum class Backend {
        None = 0x0,
        BluetoothLe = 0x01,
        WebSocket = 0x02,
        All = BluetoothLe | WebSocket
    };
    Q_DECLARE_FLAGS(Backends, Backend)
    Q_FLAG(Backends)
    */

    explicit QZeroPropsClient(QObject* parent = nullptr);
    ~QZeroPropsClient();

    //Backends supportedBackends() const;

    void setDiscoveryTimeout(int msTimeout);

    QObjectList discoveredServices() const;

public slots:
    void startDiscovery(const Configuration& config);
    void stopDiscovery();

    void connectToService(QZeroPropsService* device);
    void disconnectFromService();

signals:
    void stateChanged(State state, const QString& errorString = QString());
    void servicesChanged();

private:
    class QZeroPropsClientPrivate* const d;
};

} // namespace QZeroProps

//Q_DECLARE_OPERATORS_FOR_FLAGS(QZeroProps::QZeroPropsClient::Backends)
