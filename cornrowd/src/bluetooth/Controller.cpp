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

#include "AudioSinkAgent.h"

#include <QCoreApplication>
#include <QDebug>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusUnixFileDescriptor>

#include <BluezQt/Adapter>
#include <BluezQt/Device>
#include <BluezQt/GattApplication>
#include <BluezQt/GattCharacteristic>
#include <BluezQt/GattManager>
#include <BluezQt/GattService>
#include <BluezQt/InitManagerJob>
#include <BluezQt/LEAdvertisement>
#include <BluezQt/LEAdvertisingManager>
#include <BluezQt/Manager>
#include <BluezQt/Media>
#include <BluezQt/MediaEndpoint>
#include <BluezQt/Services>
#include <BluezQt/Types>

#include <loguru/loguru.hpp>

#include <cmath>
#include <unistd.h>

using namespace std::placeholders;

namespace bluetooth
{

using namespace BluezQt;

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    //
    QBluetoothLocalDevice localDevice;
    localDevice.setHostMode(QBluetoothLocalDevice::HostDiscoverable);

    // Init BluezQt
    m_manager = new Manager(this);
    InitManagerJob* initJob = m_manager->init();
    initJob->exec();
    if (initJob->error()) {
        LOG_F(WARNING, "Bluetooth manager failed to initialize: %s", initJob->errorText().toStdString().c_str());
        return;
    }

    // Check for useableAdapter
    if (!m_manager->usableAdapter()) {
        LOG_F(WARNING, "Bluetooth adapter not found");
        return;
    }

    // Check for media interface
    if (!m_manager->usableAdapter()->media()) {
        LOG_F(WARNING, "Bluetooth adapter does not offer media interface: %s", m_manager->usableAdapter()->name().toStdString().c_str());
        return;
    }

    AudioSinkAgent* agent = new AudioSinkAgent({Services::AdvancedAudioDistribution, Services::AudioVideoRemoteControl}, this);
    m_manager->registerAgent(agent);
    m_manager->requestDefaultAgent(agent);

    MediaEndpoint* sbcSink = new MediaEndpoint({MediaEndpoint::Role::AudioSink, MediaEndpoint::Codec::Sbc}, m_manager);
    m_manager->usableAdapter()->media()->registerEndpoint(sbcSink);
    connect(m_manager, &Manager::deviceChanged, [this](DevicePtr device) {
        connect(device.data(), &Device::mediaTransportChanged, this, &Controller::onTransportChanged);
    });

    for (auto adapter : m_manager->adapters()) {
        adapter->setDiscoverableTimeout(0);
        adapter->setPairableTimeout(0);
        adapter->setDiscoverable(true);
        adapter->setPairable(true);
    }

    initBle();
}

Controller::~Controller()
{
    m_manager->usableAdapter()->leAdvertisingManager()->unregisterAdvertisement(m_advertisement);
    m_manager->usableAdapter()->gattManager()->unregisterApplication(m_application);
}

void Controller::setReadFiltersCallback(ReadFiltersCallback callback)
{
    m_readFiltersCallback = callback;
}

void Controller::setReadIoCapsCallback(ReadIoCapsCallback callback)
{
    m_readIoCapsCallback = callback;
}

void Controller::setReadIoConfCallback(ReadIoConfCallback callback)
{
    m_readIoConfCallback = callback;
}

void Controller::initBle()
{
    if (!m_manager->usableAdapter() || !m_manager->usableAdapter()->leAdvertisingManager()) {
        LOG_F(WARNING, "No BLE adapter present");
        return;
    }

    m_advertisement = new LEAdvertisement({QStringLiteral("ad100000-d901-11e8-9f8b-f2801f1b9fd1")}, this);
    m_manager->usableAdapter()->leAdvertisingManager()->registerAdvertisement(m_advertisement);

    m_application = new GattApplication(this);
    auto service = new GattService(QStringLiteral("ad100000-d901-11e8-9f8b-f2801f1b9fd1"), true, m_application);

    auto peqCharc = new GattCharacteristic(QStringLiteral("ad10e100-d901-11e8-9f8b-f2801f1b9fd1"), service);
    peqCharc->setReadCallback(std::bind(&Controller::onReadPeqFilters, this));
    connect(peqCharc, &GattCharacteristic::valueWritten, std::bind(&Controller::onWriteFilters, this, common::ble::CharacteristicType::Peq, _1));

    auto auxCharc = new GattCharacteristic(QStringLiteral("ad10a100-d901-11e8-9f8b-f2801f1b9fd1"), service);
    auxCharc->setReadCallback(std::bind(&Controller::onReadAuxFilters, this));
    connect(auxCharc, &GattCharacteristic::valueWritten, std::bind(&Controller::onWriteFilters, this, common::ble::CharacteristicType::Aux, _1));

    auto ioCapsCharc = new GattCharacteristic(QString::fromStdString(common::ble::ioCapsCharacteristicUuid), service);
    ioCapsCharc->setReadCallback(std::bind(&Controller::onReadIoCaps, this));

    auto ioConfCharc = new GattCharacteristic(QString::fromStdString(common::ble::ioConfCharacteristicUuid), service);
    ioConfCharc->setReadCallback(std::bind(&Controller::onReadIoConf, this));
    connect(ioConfCharc, &GattCharacteristic::valueWritten, this, &Controller::onWriteIoConf);

    m_manager->usableAdapter()->gattManager()->registerApplication(m_application);
}

void Controller::onTransportChanged(MediaTransportPtr transport)
{
    if (m_transport == transport) {
        return;
    }

    if (m_transport) {
        disconnect(m_transport.data(), &MediaTransport::stateChanged, this, &Controller::onTransportStateChanged);
        disconnect(m_transport.data(), &MediaTransport::volumeChanged, this, &Controller::onTransportVolumeChanged);
    }

    m_transport = transport;
    if (!m_transport) {
        emit transportChanged(-1, 0, 0);
        return;
    }

    connect(m_transport.data(), &MediaTransport::stateChanged, this, &Controller::onTransportStateChanged);
    connect(m_transport.data(), &MediaTransport::volumeChanged, this, &Controller::onTransportVolumeChanged);
}

void Controller::onTransportStateChanged(BluezQt::MediaTransport::State state)
{
    qDebug() << "Transport state:" << state;

    switch (state) {
    case MediaTransport::State::Idle:
        // Bluez already releases transport. No need to do manually.
        //m_transport->release();
        emit transportChanged(-1, 0, 44100);
        break;
    case MediaTransport::State::Pending: {
        //QCoreApplication::processEvents();
        auto *call = m_transport->tryAcquire();
        connect(call, &PendingCall::finished, [this, call]() {
            qDebug() << __func__ << "fd: " << call->valueAt<0>().fileDescriptor() << "mtu read:" << call->valueAt<1>() << "mtu write:" << call->valueAt<2>();
            // We have to dup() the file descriptor as long as QDBusUnixFileDescriptor object is valid
            emit transportChanged(
                        ::dup(call->valueAt<0>().fileDescriptor()),
                        call->valueAt<1>(),
                        m_transport->audioConfiguration().sampleRate == BluezQt::AudioSampleRate::Rate44100 ? 44100 : 48000);
        });
        break;
    }
    case MediaTransport::State::Active:
        break;
    }
}

void Controller::onTransportVolumeChanged(uint16_t volume)
{
    qDebug() << __func__ << "> transport volume:" << volume;

    if (volume == 0) {
        emit volumeChanged(0.0f);
        return;
    }

    static const float factor = 4.0;        // 127 - 7 -> 0dB - -30dB
    //static const float factor = 2.6666;   // 127 - 7 -> 0dB - -45dB
    emit volumeChanged(pow(10.0f, (volume-127)/(factor*20.0f)));
}

QByteArray Controller::onReadPeqFilters()
{
    return m_converter.filtersToBle(m_readFiltersCallback(common::ble::CharacteristicType::Peq));
}

QByteArray Controller::onReadAuxFilters()
{
    return m_converter.filtersToBle(m_readFiltersCallback(common::ble::CharacteristicType::Aux));
}

QByteArray Controller::onReadIoCaps()
{
    return m_converter.toBle(m_readIoCapsCallback());
}

QByteArray Controller::onReadIoConf()
{
    return m_converter.toBle(m_readIoConfCallback());
}

void Controller::onWriteFilters(common::ble::CharacteristicType group, const QByteArray& value)
{
    emit filtersWritten(group, m_converter.filtersFromBle(value));
}

void Controller::onWriteIoConf(const QByteArray& value)
{
    auto interfaces = m_converter.fromBle(value);

    common::IoInterface input;
    common::IoInterface output;

    for (const auto& interface : interfaces) {
        if (interface.isOutput) {
            output = interface;
        } else {
            input = interface;
        }
    }

    emit inputSet(input);
    emit outputSet(output);
}

} // namespace bluetooth
