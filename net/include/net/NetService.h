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

#include <QtRemoteObjects/QRemoteObjectHost>

#include <common/ble/Converter.h>

namespace net
{

class NetService : public QObject
{
    Q_OBJECT

public:
    explicit NetService(QObject *parent = nullptr);
    ~NetService();

    using ReadFiltersCallback = std::function<std::vector<common::Filter>(common::ble::CharacteristicType group)>;
    void setReadFiltersCallback(ReadFiltersCallback callback);

    using ReadIoCapsCallback = std::function<std::vector<common::IoInterface>()>;
    void setReadIoCapsCallback(ReadIoCapsCallback callback);

    using ReadIoConfCallback = std::function<std::vector<common::IoInterface>()>;
    void setReadIoConfCallback(ReadIoConfCallback callback);

signals:
    void transportChanged(int fd, uint16_t blockSize, uint32_t sampleRate);
    void volumeChanged(float volume);

    void filtersWritten(common::ble::CharacteristicType group, const std::vector<common::Filter>& filters);
    void inputSet(common::IoInterface input);
    void outputSet(common::IoInterface output);

private:
    QRemoteObjectHost* m_host = nullptr;

    QByteArray onReadPeqFilters();
    QByteArray onReadAuxFilters();
    QByteArray onReadIoCaps();
    QByteArray onReadIoConf();
    void onWriteFilters(common::ble::CharacteristicType group, const QByteArray& value);
    void onWriteIoConf(const QByteArray& value);

    ReadFiltersCallback m_readFiltersCallback = nullptr;
    ReadIoCapsCallback  m_readIoCapsCallback = nullptr;
    ReadIoConfCallback  m_readIoConfCallback = nullptr;
    common::ble::Converter m_converter;
};

} // namespace net
