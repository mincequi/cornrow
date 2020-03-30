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

#include "Persistence.h"

#include "../audio/Controller.h"
#include "../bluetooth/Controller.h"

#include <loguru/loguru.hpp>
#include <net/TcpServer.h>

#include <QUuid>

using namespace std::placeholders;

namespace config
{

Controller::Controller(audio::Controller* audio,
                       bluetooth::Controller* bluetooth,
                       net::TcpServer* tcpServer,
                       QObject* parent)
    : QObject(parent),
      m_audio(audio),
      m_bluetooth(bluetooth),
      m_tcpServer(tcpServer)
{
    // On start-up we read config from disk
    std::vector<common::Filter> filters = m_persistence.readConfig();
    std::vector<common::Filter> peqFilters, auxFilters;
    for (const auto& filter : filters) {
        if (filter.type >= common::FilterType::Peak && filter.type <= common::FilterType::AllPass) {
            peqFilters.push_back(filter);
        } else if (filter.type >= common::FilterType::CrossoverLr2 && filter.type <= common::FilterType::Loudness){
            auxFilters.push_back(filter);
        }
    }

    m_audio->setFilters(common::ble::CharacteristicType::Peq, peqFilters);
    m_audio->setFilters(common::ble::CharacteristicType::Aux, auxFilters);

    m_bluetooth->setReadFiltersCallback(std::bind(&audio::Controller::filters, m_audio, _1));
    connect(m_bluetooth, &bluetooth::Controller::filtersWritten, m_audio, &audio::Controller::setFilters);

    m_tcpServer->setProperty(QByteArray(common::ble::peqCharacteristicUuid.c_str()), m_converter.filtersToBle(peqFilters));
    m_tcpServer->setProperty(QByteArray(common::ble::auxCharacteristicUuid.c_str()), m_converter.filtersToBle(auxFilters));

    //m_tcpServer->setReadCallback();
    connect(m_tcpServer, &net::TcpServer::propertyChanged, [this](const QUuid& name, const QByteArray& value) {
        auto uuid = name.toByteArray(QUuid::WithoutBraces).toStdString();
        if (uuid == common::ble::peqCharacteristicUuid) {
            m_audio->setFilters(common::ble::CharacteristicType::Peq, m_converter.filtersFromBle(value));
        } else if (uuid == common::ble::auxCharacteristicUuid) {
            m_audio->setFilters(common::ble::CharacteristicType::Aux, m_converter.filtersFromBle(value));
        } else {
            LOG_F(WARNING, "Unknown uuid: %s", name.toByteArray(QUuid::WithoutBraces).toStdString().c_str());
        }
    });

    m_bluetooth->setReadIoCapsCallback(std::bind(&audio::Controller::ioCaps, m_audio));
    m_bluetooth->setReadIoConfCallback(std::bind(&audio::Controller::ioConf, m_audio));
    connect(m_bluetooth, &bluetooth::Controller::inputSet, m_audio, &audio::Controller::setInput);
    connect(m_bluetooth, &bluetooth::Controller::outputSet, m_audio, &audio::Controller::setOutput);
}

Controller::~Controller()
{
}

void Controller::writeConfig()
{
    // If connection is closed, we write config to disk
    auto peqFilters = m_audio->filters(common::ble::CharacteristicType::Peq);
    auto auxFilters = m_audio->filters(common::ble::CharacteristicType::Aux);

    std::vector<common::Filter> filters;
    filters.insert(filters.end(),
                   peqFilters.begin(),
                   peqFilters.end());
    filters.insert(filters.end(),
                   auxFilters.begin(),
                   auxFilters.end());

    m_persistence.writeConfig(filters);
}

} // namespace config
