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

#include <common/RemoteDataStore.h>
#include <common/ble/Converter.h>

namespace net
{

class NetService : public QObject
{
    Q_OBJECT

public:
    explicit NetService(common::IAudioConf* audio, QObject *parent = nullptr);
    ~NetService();

    void disconnect();


signals:
    void transportChanged(int fd, uint16_t blockSize, uint32_t sampleRate);
    void volumeChanged(float volume);

    void inputSet(common::IoInterface input);
    void outputSet(common::IoInterface output);

private:
    void onDataReceived();
    void onWriteIoConf(const QByteArray& value);

    common::RemoteDataStore* m_remoteData = nullptr;

    QTcpServer  m_tcpServer;
    QTcpSocket* m_socket = nullptr;
    QDataStream m_inStream;

    common::ble::Converter m_converter;
};

} // namespace net
