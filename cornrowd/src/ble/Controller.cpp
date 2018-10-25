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

#include <QtBluetooth/QLowEnergyController>

#include <QtBluetooth/qlowenergyadvertisingdata.h>
#include <QtBluetooth/qlowenergyadvertisingparameters.h>
#include <QtBluetooth/qlowenergycharacteristic.h>
#include <QtBluetooth/qlowenergycharacteristicdata.h>
#include <QtBluetooth/qlowenergydescriptordata.h>
#include <QtBluetooth/qlowenergycontroller.h>
#include <QtBluetooth/qlowenergyservice.h>
#include <QtBluetooth/qlowenergyservicedata.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qlist.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qtimer.h>

namespace ble
{

struct ControllerPrivate
{
    ControllerPrivate(Controller* q)
    {
        peripheral = QLowEnergyController::createPeripheral(q);

        advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
        advertisingData.setIncludePowerLevel(true);
        advertisingData.setLocalName("CornRow");
        advertisingData.setServices(QList<QBluetoothUuid>() << QBluetoothUuid::CurrentTimeService);
    }

    QLowEnergyController* peripheral;
    QLowEnergyAdvertisingData advertisingData;
};

Controller::Controller(QObject *parent)
    : QObject(parent),
      d(new ControllerPrivate(this))
{
    QLowEnergyCharacteristicData charData;
    charData.setUuid(QBluetoothUuid::BodySensorLocation);
    //charData.setUuid(QBluetoothUuid(quint16(0x2a3a)));
    charData.setValue(QByteArray(2, 0));
    charData.setProperties(QLowEnergyCharacteristic::Notify | QLowEnergyCharacteristic::Read);
    const QLowEnergyDescriptorData clientConfig(QBluetoothUuid::ClientCharacteristicConfiguration,
                                                QByteArray(2, 0));
    charData.addDescriptor(clientConfig);

    // Service
    QLowEnergyServiceData serviceData;
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(QBluetoothUuid::BodySensorLocation);
    serviceData.addCharacteristic(charData);

    const auto service = d->peripheral->addService(serviceData);
    // Characteristic
    QByteArray value;
    value.append(char(0)); // Flags that specify the format of the value.
    value.append(char(70)); // Actual value.
    QLowEnergyCharacteristic characteristic = service->characteristic(QBluetoothUuid(quint16(0x2a3a)));
    service->writeCharacteristic(characteristic, value); // Potentially causes notification.


    //! [Provide Heartbeat]
    /*
    QTimer heartbeatTimer;
    quint8 currentHeartRate = 60;
    enum ValueChange { ValueUp, ValueDown } valueChange = ValueUp;
    const auto heartbeatProvider = [&service, &currentHeartRate, &valueChange]() {
        QByteArray value;
        value.append(char(0)); // Flags that specify the format of the value.
        value.append(char(currentHeartRate)); // Actual value.
        QLowEnergyCharacteristic characteristic = service->characteristic(QBluetoothUuid::HeartRateMeasurement);
        Q_ASSERT(characteristic.isValid());
        service->writeCharacteristic(characteristic, value); // Potentially causes notification.
        if (currentHeartRate == 60)
            valueChange = ValueUp;
        else if (currentHeartRate == 100)
            valueChange = ValueDown;
        if (valueChange == ValueUp)
            ++currentHeartRate;
        else
            --currentHeartRate;
    };
    QObject::connect(&heartbeatTimer, &QTimer::timeout, heartbeatProvider);
    heartbeatTimer.start(1000);
    //! [Provide Heartbeat]
    */

    // Advertising will stop once a client connects, so re-advertise once disconnected.
    QObject::connect(d->peripheral, &QLowEnergyController::disconnected, this, &Controller::startAdvertising);
    startAdvertising();
}

Controller::~Controller()
{
    delete d;
}

void Controller::startAdvertising()
{
    d->peripheral->startAdvertising(QLowEnergyAdvertisingParameters(), d->advertisingData, d->advertisingData);
}

} // namespace ble
