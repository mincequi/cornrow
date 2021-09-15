/*
 * Copyright (C) 2021 Manuel Weichselbaumer <mincequi@web.de>
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

#include "Config.h"

#define TOML_EXCEPTIONS 0
#include "toml/toml.hpp"

Config::Config(const std::string& configFile) {
    if (!configFile.empty()) {
        m_configFile = configFile;
    }
}

void Config::parse() {
    auto result = toml::parse_file(m_configFile);
    if (!result) {
        return;
    }

    // Only check if bluetooth_source is present
    if (result.table()["bluetooth_source"]) {
        m_pipelineConfig.bluetoothConfig = coro::bluetooth::BluetoothSource::Config();
    }

    if (result.table()["airplay_source"]) {
        m_pipelineConfig.airplayConfig = coro::airplay::AirplaySource::Config();
        m_pipelineConfig.airplayConfig->name = result.table()["airplay_source"]["name"].value_or("myAirplay");
        m_pipelineConfig.airplayConfig->port = result.table()["airplay_source"]["port"].value_or(0);
        m_pipelineConfig.airplayConfig->bufferTimeMs = result.table()["airplay_source"]["buffer_time"].value_or(2000);
    }

    if (result.table()["tcp_sink"]) {
        m_pipelineConfig.tcpConfig = coro::core::TcpClientSink::Config();
        m_pipelineConfig.tcpConfig->host = result.table()["tcp_sink"]["host"].value_or("127.0.0.1");
        m_pipelineConfig.tcpConfig->port = result.table()["tcp_sink"]["port"].value_or(4953);
    }
}

CoroPipeline::Config Config::pipelineConfig() const {
    return m_pipelineConfig;
}
