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

#include <common/ble/Converter.h>

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

    using ReadFiltersCallback = std::function<std::vector<common::Filter>(common::ble::CharacteristicType group)>;
    void setReadFiltersCallback(ReadFiltersCallback callback);

    using ReadIoCapsCallback = std::function<std::vector<common::IoInterface>()>;
    void setReadIoCapsCallback(ReadIoCapsCallback callback);

    using ReadIoConfCallback = std::function<std::vector<common::IoInterface>()>;
    void setReadIoConfCallback(ReadIoConfCallback callback);

signals:
    void transportChanged(int fd, uint16_t blockSize);
    void volumeChanged(float volume);

    void filtersWritten(common::ble::CharacteristicType group, const std::vector<common::Filter>& filters);
    void inputSet(common::IoInterface input);
    void outputSet(common::IoInterface output);

private:
    void initBle();

    void onTransportChanged(BluezQt::MediaTransportPtr transport);
    void onTransportStateChanged(BluezQt::MediaTransport::State state);
    void onTransportVolumeChanged(uint16_t volume);
    QByteArray onReadPeqFilters();
    QByteArray onReadAuxFilters();
    QByteArray onReadIoCaps();
    QByteArray onReadIoConf();
    void onWriteFilters(common::ble::CharacteristicType group, const QByteArray& value);
    void onWriteIoConf(const QByteArray& value);

    BluezQt::Manager* m_manager = nullptr;
    BluezQt::GattApplication* m_application = nullptr;
    BluezQt::LEAdvertisement* m_advertisement = nullptr;
    BluezQt::MediaTransportPtr m_transport = nullptr;
    ReadFiltersCallback m_readFiltersCallback = nullptr;
    ReadIoCapsCallback  m_readIoCapsCallback = nullptr;
    ReadIoConfCallback  m_readIoConfCallback = nullptr;
    common::ble::Converter m_converter;
};

} // namespace bluetooth
