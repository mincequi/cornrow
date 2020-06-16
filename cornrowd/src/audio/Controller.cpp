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

#include "Controller.h"

#include "CoroPipeline.h"

#include <common/Types.h>
#include <coro/core/Mainloop.h>

#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QtDBus/QDBusObjectPath>

#include <loguru/loguru.hpp>
#include <unistd.h>

namespace audio {

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    m_coroPipeline = new CoroPipeline();

    startTimer(1);
}

Controller::~Controller()
{
    delete m_coroPipeline;
}

std::vector<common::Filter> Controller::filters(common::ble::CharacteristicType group) const
{
    return m_filters.at(group);
}

void Controller::setFilters(common::ble::CharacteristicType group, const std::vector<common::Filter>& filters)
{
    for (const auto& filter : filters) {
        qDebug() << "group:" << static_cast<uint>(group) <<
                    "type:" << static_cast<uint>(filter.type) <<
                    ", f:" << filter.f <<
                    ", g:" << filter.g <<
                    ", q:" << filter.q;
    }
    qDebug() << "";

    // Store filters locally
    m_filters[group] = filters;

    switch (group) {
    case common::ble::CharacteristicType::Peq:
        m_coroPipeline->setPeq(filters);
        break;
    case common::ble::CharacteristicType::Aux: {
        // Check if crossover was provided, if not, we disable crossover
        auto it = std::find_if(filters.begin(), filters.end(), [](const common::Filter& f) {
            return f.type == common::FilterType::CrossoverLr2 ||
                    f.type == common::FilterType::CrossoverLr4;
        });
        it != filters.end() ? m_coroPipeline->setCrossover(*it) : m_coroPipeline->setCrossover(common::Filter());

        // Check if loudness was provided
        it = std::find_if(filters.begin(), filters.end(), [](const common::Filter& f) {
            return f.type == common::FilterType::Loudness;
        });
        it != filters.end() ? m_coroPipeline->setLoudness(static_cast<uint8_t>(it->g)) : m_coroPipeline->setLoudness(0);
        break;
    }
    default:
        break;
    }
}

std::vector<common::IoInterface> Controller::ioCaps()
{
    m_outputDeviceMap.clear();

    // Populate output device map (IoInterface struct to string).
    auto devices = m_alsaUtil.outputDevices();
    for (const auto& d : devices) {
        switch (d.type) {
        case coro::AudioDeviceType::Default:
            m_outputDeviceMap.insert( { common::IoInterfaceType::Default, d.name } );
            break;
        case coro::AudioDeviceType::Spdif:
            m_outputDeviceMap.insert( { common::IoInterfaceType::Spdif, d.name } );
            break;
        case coro::AudioDeviceType::Hdmi:
            m_outputDeviceMap.insert( { common::IoInterfaceType::Hdmi, d.name } );
            break;
        default:
            break;
        }
    }

    if (m_coroPipeline->hasPiHdmiOutput()) {
        m_outputDeviceMap.insert( { common::IoInterfaceType::Hdmi, "Raspberry Pi HDMI" } );
    }

    std::vector<common::IoInterface> ioCaps = {
        { common::IoInterfaceType::Bluetooth, false, 1 },
        { common::IoInterfaceType::Scream,    false, 1 }
    };

    for (auto it = m_outputDeviceMap.begin(); it != m_outputDeviceMap.end(); it = m_outputDeviceMap.upper_bound(it->first)) {
        ioCaps.push_back( { it->first, true, m_outputDeviceMap.count(it->first) });
    }

    return ioCaps;
}

std::vector<common::IoInterface> Controller::ioConf() const
{
    return { m_input, m_output };
}

void Controller::setInput(const common::IoInterface& interface)
{
    m_input = interface;
}

void Controller::setOutput(const common::IoInterface& interface)
{
    m_output = interface;
    // Find all devices of interface type
    auto range = m_outputDeviceMap.equal_range(interface.type);
    // Check if index is in valid range
    auto count = std::distance(range.first, range.second);
    if (count <= interface.number) {
        LOG_F(WARNING, "Output device invalid. type: %d, index: %d", static_cast<int>(interface.type), static_cast<int>(interface.number));
        return;
    }

    // Advance iterator to selected device and set device name accordingly
    auto it = range.first;
    std::advance(it, interface.number);
    LOG_F(INFO, "Output device set to: %s", it->second.c_str());
    m_coroPipeline->setOutputDevice(it->second);
}

void Controller::timerEvent(QTimerEvent* event)
{
    coro::core::Mainloop::instance().poll();
}

void Controller::setTransport(int fd, uint16_t blockSize, int rate)
{
    LOG_F(INFO, "set transport> fd: %d, blocksize: %d, rate: %d", fd, blockSize, rate);
    m_coroPipeline->setFileDescriptor(fd, blockSize);
}

void Controller::setVolume(float volume)
{
    m_coroPipeline->setVolume(volume);
}

} // namespace audio
