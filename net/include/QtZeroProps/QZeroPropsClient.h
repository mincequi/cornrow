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

#include <QtZeroProps/QZeroPropsTypes.h>

#include <QObject>

namespace QtZeroProps
{
class QZeroPropsService;

/// Discovers and connects to a remote QZeroPropsServer.
class QZeroPropsClient : public QObject
{
    Q_OBJECT

    /// Returns a list of all discovered services.
    Q_PROPERTY(QList<QObject*> discoveredServices READ discoveredServices NOTIFY servicesChanged)

public:
    /// This enum describes the different states in which a client can be.
    enum class State : uint8_t {
        Idle,           ///< The client is idle. We also enter this state when Discovering times out.
        Discovering,    ///< The client is disovering services.
        Connecting,     ///< The client is connecting to a service.
        Connected,      ///< The client is connected to a service.
        Disconnected,   ///< The connection has been interrupted.
        Error           ///< This indicated that an error occured.
    };
    Q_ENUM(State)

    /// Constructs a new QZeroPropsClient with parent.
    explicit QZeroPropsClient(QObject* parent = nullptr);

    /// Destroys this QZeroPropsClient.
    ~QZeroPropsClient();

    //ServiceTypes supportedServiceTypes() const;

    /// Sets the maximum search time for QZeroPropsServers in milliseconds.
    /// If timeout is 0 the discovery runs until stopDiscovery() is called.
    ///
    /// The timeout ensures that the discovery is stopped after timeout
    /// milliseconds. It is still possible to manually abort the discovery by
    /// calling stopDiscovery().
    ///
    /// The new timeout value does not take effect until the discovery is
    /// restarted.
    void setDiscoveryTimeout(int msTimeout);

    /// Returns a list of all discovered services. See QZeroPropsService for
    /// object details.
    ///
    /// When a new discovery is started, all previously returned
    /// QZeroPropsServices are invalid.
    QObjectList discoveredServices() const;

public slots:
    /// Starts QZeroProps service discovery. If it was previously started, it
    /// will be stopped and started again. Any previously returned
    /// QZeroPropsService will be invalid.
    ///
    /// The Discovering state is emitted once the discovery is started. When it
    /// finishes, the Idle state is emitted.
    ///
    /// The configuration defines the Bluetooth Low Energy and WebSocket
    /// services we are looking for. If the appropriate fields are empty or
    /// undefined, they are not discovered.
    void startDiscovery(const ServiceConfiguration& config);

    /// Stops QZeroProps service discovery.
    ///
    /// The Idle state is emitted once the discovery is stopped.
    void stopDiscovery();

    /// Connect to a QZeroPropsService that has previously been discovered.
    void connectToService(QZeroPropsService* device);

    /// Disconnect from currently connected QZeroPropsService.
    void disconnectFromService();

signals:
    /// This signal is emitted whenever QZeroPropsClient's state changes.
    ///
    /// The state parameter is the new state.
    /// The errorString is additionally provided in case an Error state occured.
    void stateChanged(State state, const QString& errorString = QString());

    /// This signal is emitted whenever discovered services changed during
    /// Discovering state. It is also emitted when a new discovery is started
    /// and all previously discovered services are cleared.
    void servicesChanged();

private:
    class QZeroPropsClientPrivate* const d;
};

} // namespace QZeroProps
