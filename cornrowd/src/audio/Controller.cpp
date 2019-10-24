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

#include <QDebug>
#include <QThread>
#include <QtDBus/QDBusObjectPath>

#include <gstreamermm-dsp.h>
#include <common/Types.h>

#include "Pipeline.h"

#include <unistd.h>

namespace audio
{

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    // Init gstreamermm-dsp
    GstDsp::init();

    //m_normalPipeline = new Pipeline(Pipeline::Type::Normal);
    //m_crossoverPipeline = new Pipeline(Pipeline::Type::Crossover);
    m_currentPipeline = new Pipeline(Pipeline::Type::Normal);;
}

Controller::~Controller()
{
    //delete m_normalPipeline;
    //delete m_crossoverPipeline;
    delete m_currentPipeline;
}

std::vector<common::Filter> Controller::filters(common::ble::CharacteristicType group)
{
    return m_filters[group];
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
        m_currentPipeline->setPeq(filters);
        break;
    case common::ble::CharacteristicType::Aux: {
        updatePipeline();
        // Check if crossover was provided, if not, we disable crossover
        auto it = std::find_if(filters.begin(), filters.end(), [](const common::Filter& f) {
            return f.type == common::FilterType::Crossover;
        });
        it != filters.end() ? m_currentPipeline->setCrossover(*it) : m_currentPipeline->setCrossover(common::Filter());
        // Check if loudness was provided
        it = std::find_if(filters.begin(), filters.end(), [](const common::Filter& f) {
            return f.type == common::FilterType::Loudness;
        });
        it != filters.end() ? m_currentPipeline->setLoudness(static_cast<uint8_t>(it->g)) : m_currentPipeline->setLoudness(0);
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
        case GstDsp::AudioDeviceType::Default:
            m_outputDeviceMap.insert( { common::IoInterfaceType::Default, d.name } );
            break;
        case GstDsp::AudioDeviceType::Spdif:
            m_outputDeviceMap.insert( { common::IoInterfaceType::Spdif, d.name } );
            break;
        default:
            break;
        }
    }

    std::vector<common::IoInterface> ioCaps = {
        { common::IoInterfaceType::Bluetooth, false, 1 }
    };

    for (auto it = m_outputDeviceMap.begin(); it != m_outputDeviceMap.end(); ++it) {
        ioCaps.push_back( { it->first, true, m_outputDeviceMap.count(it->first) });
        it == m_outputDeviceMap.upper_bound(it->first);
    }

    return ioCaps;
}

std::vector<common::IoInterface> Controller::ioConf()
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
    if (count <= interface.index) {
        qDebug() << "invalid output interface> type:" << static_cast<int>(interface.type) << ", index:" << static_cast<int>(interface.index);
        return;
    }

    // Advance iterator to selected device and set device name accordingly
    auto it = range.first;
    std::advance(it, interface.index);
    m_currentPipeline->setOutputDevice(it->second);
}

void Controller::setTransport(int fd, uint16_t blockSize, int rate)
{
    //::close(m_fd);
    m_fd = fd;
    m_blockSize = blockSize;
    m_rate = rate;
    m_currentPipeline->setTransport(m_fd, m_blockSize, m_rate);
}

void Controller::setVolume(float volume)
{
    m_currentPipeline->setVolume(volume);
}

void Controller::updatePipeline()
{
    // Check if a crossover is set
    auto it = std::find_if(m_filters[common::ble::CharacteristicType::Aux].begin(),
            m_filters[common::ble::CharacteristicType::Aux].end(),
            [](const common::Filter& f) {
        return f.type == common::FilterType::Crossover;
    });

    // Check if current pipeline is desired type
    Pipeline::Type type = (it != m_filters[common::ble::CharacteristicType::Aux].end()) ? Pipeline::Type::Crossover : Pipeline::Type::Normal;
    if (type == m_currentPipeline->type()) {
        return;
    }

    // We want another pipeline, stop current one
    m_currentPipeline->setTransport(-1, 0, 0);
    delete m_currentPipeline;

    switch (type) {
    case Pipeline::Type::Normal:
        m_currentPipeline = new Pipeline(Pipeline::Type::Normal);
        break;
    case Pipeline::Type::Crossover:
        m_currentPipeline = new Pipeline(Pipeline::Type::Crossover);
        break;
    }

    m_currentPipeline->setCrossover((it != m_filters[common::ble::CharacteristicType::Aux].end()) ? *it : common::Filter());
    m_currentPipeline->setPeq(m_filters[common::ble::CharacteristicType::Peq]);
    //m_currentPipeline->setTransport(m_transport);
    m_currentPipeline->setTransport(m_fd, m_blockSize, m_rate);
}

} // namespace audio
