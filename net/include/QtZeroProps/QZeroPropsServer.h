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

#include <QObject>

namespace QtZeroProps
{
class QZeroPropsService;
struct ServiceConfiguration;

/// QZeroPropsServer creates, starts and announces QZeroPropsServices which can
/// be connected to from remote QZeroPropsClients.
class QZeroPropsServer : public QObject
{
    Q_OBJECT

public:
    /// Constructs a new QZeroPropsServer with parent.
    explicit QZeroPropsServer(QObject* parent = nullptr);

    /// Destroys this QZeroPropsServer.
    ~QZeroPropsServer();

    //ServiceTypes supportedServiceTypes() const;

    /// Create, start and announce a QZeroPropsService represented by
    /// configuration.
    ///
    /// Any QZeroPropsService instance that resulted from previous call to this
    /// function is invalidated.
    ///
    /// The configuration defines the Bluetooth Low Energy and WebSocket
    /// services we are providing. If the appropriate fields are empty or
    /// undefined, the corresponding backend is not started.
    QZeroPropsService* startService(const ServiceConfiguration& configuration);

    /// Stops current QZeroProps service.
    void stopService();

private:
    class QZeroPropsServerPrivate* const d;
};

} // namespace QZeroProps
