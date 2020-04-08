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

#include <QObject>

#include "Persistence.h"

#include <common/ble/Converter.h>

namespace audio {
class Controller;
}

namespace bluetooth {
class Controller;
}

namespace net {
class QZeroPropsWsServer;
}

namespace config {

class Controller : public QObject
{
    Q_OBJECT

public:
    explicit Controller(audio::Controller* audio,
                        bluetooth::Controller* bluetooth,
                        net::QZeroPropsWsServer* tcpServer,
                        QObject* parent = nullptr);
    ~Controller();

    void writeConfig();

private:
    audio::Controller* m_audio = nullptr;
    bluetooth::Controller* m_bluetooth = nullptr;
    net::QZeroPropsWsServer* m_tcpServer = nullptr;

    Persistence m_persistence;

    common::ble::Converter m_converter;
};

} // namespace config
