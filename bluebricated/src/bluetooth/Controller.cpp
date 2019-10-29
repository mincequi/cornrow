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
        qWarning() << "Error initializing bluetooth manager:" << initJob->errorText();
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
}

Controller::~Controller()
{
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
        m_fd = {};
        emit transportChanged(m_fd.fileDescriptor(), 0);
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
        break;
    case MediaTransport::State::Pending: {
        //QCoreApplication::processEvents();
        auto *call = m_transport->tryAcquire();
        connect(call, &PendingCall::finished, [this, call]() {
            m_fd = call->valueAt<0>();
            qDebug() << __func__ << "fd: " << m_fd.fileDescriptor() << "mtu read:" << call->valueAt<1>() << "mtu write:" << call->valueAt<2>();
            emit transportChanged(m_fd.fileDescriptor(), call->valueAt<1>());
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

} // namespace bluetooth
