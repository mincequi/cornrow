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

#include "QZeroPropsServicePrivate.h"

#include <QWebSocket>

namespace QZeroProps
{

class QZeroPropsWebSocketService : public QZeroPropsServicePrivate
{
public:
    QZeroPropsWebSocketService(QZeroPropsService* _q);
    ~QZeroPropsWebSocketService() override;

private:
    void connect() override;
    void disconnect() override;
    void doSend(const QVariant& uuid, const QByteArray& value) override;

    // Event handlers from socket
    void onStateChanged(QAbstractSocket::SocketState state);
    void onReceive(const QByteArray& message);

    QWebSocket  m_socket;
};

} // namespace QZeroProps
