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
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtDBus/QDBusObjectPath>

#include <BluezQt/Device>
#include <BluezQt/InitManagerJob>
#include <BluezQt/Manager>
#include <BluezQt/Media>
#include <BluezQt/MediaEndpoint>
#include <BluezQt/Services>
#include <BluezQt/Types>

#include "AudioSinkAgent.h"

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
    Manager* manager = new Manager(this);
    InitManagerJob *initJob = manager->init();
    initJob->exec();
    if (initJob->error()) {
        qWarning() << "Error initializing bluetooth manager:" << initJob->errorText();
        return;
    }

    AudioSinkAgent* agent = new AudioSinkAgent({Services::AdvancedAudioDistribution, Services::AudioVideoRemoteControl}, this);
    manager->registerAgent(agent);
    manager->requestDefaultAgent(agent);

    MediaEndpoint *sbcSink = new MediaEndpoint({MediaEndpoint::Role::AudioSink, MediaEndpoint::Codec::Sbc}, manager);
    connect(sbcSink, &MediaEndpoint::configurationSet, this, &Controller::onConfigurationSet);
    connect(sbcSink, &MediaEndpoint::configurationCleared, this, &Controller::onConfigurationCleared);
    manager->media()->registerEndpoint(sbcSink);
}

void Controller::onConfigurationSet(const QString& transportObjectPath, const QVariantMap& properties)
{
    qDebug() << "Set configuration for transport:" << transportObjectPath << "to:" << properties;
    emit configurationSet(QDBusObjectPath(transportObjectPath));
}

void Controller::onConfigurationCleared(const QString& transportObjectPath)
{
    qDebug() << "Cleared configuration for transport:" << transportObjectPath;
    emit configurationCleared(QDBusObjectPath(transportObjectPath));
}

} // namespace bluetooth
