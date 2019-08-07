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

#include <ble/Converter.h>

#include <BluezQt/MediaTransport>
#include <BluezQt/Types>

class QDBusObjectPath;

namespace BluezQt
{
class GattCharacteristic;
class LEAdvertisement;
}

namespace bluetooth
{

class Controller : public QObject
{
    Q_OBJECT

public:
    explicit Controller(QObject *parent = nullptr);
    ~Controller();

    using ReadFiltersCallback = std::function<std::vector<common::Filter>(common::FilterGroup group)>;
    void setReadFiltersCallback(ReadFiltersCallback callback);

signals:
    void transportChanged(int fd, uint16_t blockSize);
    void volumeChanged(float volume);

    void filtersWritten(common::FilterGroup group, const std::vector<common::Filter>& filters);

private:
    void initBle();

    void onTransportChanged(BluezQt::MediaTransportPtr transport);
    void onTransportStateChanged(BluezQt::MediaTransport::State state);
    void onTransportVolumeChanged(uint16_t volume);
    QByteArray onReadFilters(common::FilterGroup group);
    void onWriteFilters(common::FilterGroup group, const QByteArray& value);

    BluezQt::Manager* m_manager = nullptr;
    std::map<common::FilterGroup, BluezQt::GattCharacteristic*> m_charcs;
    BluezQt::GattApplication* m_application = nullptr;
    BluezQt::LEAdvertisement* m_advertisement = nullptr;
    BluezQt::MediaTransportPtr m_transport = nullptr;
    ReadFiltersCallback m_readCallback = nullptr;
    ble::Converter m_converter;
};

} // namespace bluetooth
