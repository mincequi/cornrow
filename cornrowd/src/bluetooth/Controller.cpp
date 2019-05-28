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
    InitManagerJob *initJob = m_manager->init();
    initJob->exec();
    if (initJob->error()) {
        qWarning() << "Error initializing bluetooth manager:" << initJob->errorText();
        return;
    }

    AudioSinkAgent* agent = new AudioSinkAgent({Services::AdvancedAudioDistribution, Services::AudioVideoRemoteControl}, this);
    m_manager->registerAgent(agent);
    m_manager->requestDefaultAgent(agent);

    MediaEndpoint *sbcSink = new MediaEndpoint({MediaEndpoint::Role::AudioSink, MediaEndpoint::Codec::Sbc}, m_manager);
    connect(sbcSink, &MediaEndpoint::configurationSet, this, &Controller::onConfigurationSet);
    connect(sbcSink, &MediaEndpoint::configurationCleared, this, &Controller::onConfigurationCleared);
    m_manager->usableAdapter()->media()->registerEndpoint(sbcSink);

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
    m_readCallback = callback;
}

void Controller::initBle()
{
    m_advertisement = new LEAdvertisement({QStringLiteral("ad100000-d901-11e8-9f8b-f2801f1b9fd1")}, this);
    m_manager->usableAdapter()->leAdvertisingManager()->registerAdvertisement(m_advertisement);

    m_application = new GattApplication(this);
    auto service = new GattService(QStringLiteral("ad100000-d901-11e8-9f8b-f2801f1b9fd1"), true, m_application);

    auto peqCharc = new GattCharacteristic(QStringLiteral("ad10e100-d901-11e8-9f8b-f2801f1b9fd1"), service);
    peqCharc->setReadCallback(std::bind(&Controller::onReadFilters, this, common::FilterGroup::Peq));
    connect(peqCharc, &GattCharacteristic::valueWritten, std::bind(&Controller::onWriteFilters, this, common::FilterGroup::Peq, _1));
    m_charcs[common::FilterGroup::Peq] = peqCharc;

    auto auxCharc = new GattCharacteristic(QStringLiteral("ad10a100-d901-11e8-9f8b-f2801f1b9fd1"), service);
    auxCharc->setReadCallback(std::bind(&Controller::onReadFilters, this, common::FilterGroup::Aux));
    connect(auxCharc, &GattCharacteristic::valueWritten, std::bind(&Controller::onWriteFilters, this, common::FilterGroup::Aux, _1));
    m_charcs[common::FilterGroup::Aux] = auxCharc;

    m_manager->usableAdapter()->gattManager()->registerApplication(m_application);
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

QByteArray Controller::onReadFilters(common::FilterGroup group)
{
    return m_converter.filtersToBle(m_readCallback(group));
}

void Controller::onWriteFilters(common::FilterGroup group, const QByteArray& value)
{
    emit filtersWritten(group, m_converter.filtersFromBle(value));
}

} // namespace bluetooth
