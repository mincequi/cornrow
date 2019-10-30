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

#include "Pipeline.h"

#include <common/Types.h>
#include <common/ble/Types.h>
#include <gstdsp/AlsaUtil.h>

#include <QObject>

namespace audio
{
class Pipeline;

class Controller : public QObject
{
    Q_OBJECT

public:
    explicit Controller(QObject* parent = nullptr);
    ~Controller();

    void setTransport(int fd, uint16_t blockSize, int rate);

    void setVolume(float volume);

private:
    void updatePipeline();

    Pipeline* m_pipeline = nullptr;

    std::string m_transport;
    int m_fd = -1;
    uint16_t m_blockSize = 0;
    int m_rate = 44100;

    std::map<common::ble::CharacteristicType, std::vector<common::Filter>> m_filters;

    GstDsp::AlsaUtil m_alsaUtil;
    std::multimap<common::IoInterfaceType, std::string> m_outputDeviceMap;

    common::IoInterface m_input;
    common::IoInterface m_output;
};

} // namespace audio
