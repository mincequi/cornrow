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

#include <functional>

#include <QDataStream>
#include <QTcpServer>

namespace net
{

class TcpServer : public QObject
{
    Q_OBJECT

public:
    explicit TcpServer(QObject *parent = nullptr);
    ~TcpServer();

    void startPublishing();

    void disconnect();

    void setProperty(const char* name, const QByteArray& value);

signals:
    void propertyChanged(const char* name, const QByteArray& value);

private:
    void onClientConnected();
    // Property related event handlers / action
    void onReceive();
    void doSend(const char* name);

    QTcpServer  m_tcpServer;
    QTcpSocket* m_socket = nullptr;
    QDataStream m_inStream;
};

} // namespace net
