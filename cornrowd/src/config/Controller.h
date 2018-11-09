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

#include <ble/Peripheral.h>
#include <ble/PeripheralAdapter.h>

#include "../audio/Controller.h"

namespace config
{

class Controller : public QObject
{
    Q_OBJECT

public:
    explicit Controller(QObject *parent = nullptr);

    void setAudioController(audio::Controller* audio);
    void unsetAudioController();

private:
    void onPeqChanged(const std::vector<common::Filter>& filters);

    std::vector<common::Filter> m_peq;

    ble::Peripheral* m_ble = nullptr;
    ble::PeripheralAdapter* m_bleAdapter = nullptr;

    audio::Controller* m_audio = nullptr;
};

} // namespace config