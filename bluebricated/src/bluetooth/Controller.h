/*
 * Copyright (C) 2018 Manuel Weichselbaumer <mincequi@web.de>
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

#include <QObject>
#include <QtDBus/QDBusUnixFileDescriptor>

#include <BluezQt/MediaTransport>
#include <BluezQt/Types>

namespace bluetooth
{

class Controller : public QObject
{
    Q_OBJECT

public:
    explicit Controller(QObject *parent = nullptr);
    ~Controller();

signals:
    void transportChanged(int fd, uint16_t blockSize);
    void volumeChanged(float volume);

private:
    void onTransportChanged(BluezQt::MediaTransportPtr transport);
    void onTransportStateChanged(BluezQt::MediaTransport::State state);
    void onTransportVolumeChanged(uint16_t volume);

    BluezQt::Manager* m_manager = nullptr;
    BluezQt::MediaTransportPtr m_transport = nullptr;

    QDBusUnixFileDescriptor m_fd;
};

} // namespace bluetooth
