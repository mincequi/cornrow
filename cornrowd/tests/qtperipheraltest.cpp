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

#include <common/ble/Types.h>

int main(int argc, char *argv[])
{
    //QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth* = true"));
    QCoreApplication app(argc, argv);

    QLowEnergyAdvertisingData advertisingData;
    advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    advertisingData.setLocalName("cornrowd");
    advertisingData.setServices(QList<QBluetoothUuid>() << QBluetoothUuid(QString("ad100000-d901-11e8-9f8b-f2801f1b9fd1")));

    QLowEnergyCharacteristicData charData;
    charData.setUuid(QBluetoothUuid(QString("ad10e100-d901-11e8-9f8b-f2801f1b9fd1")));
    charData.setValue(QByteArray(4, 0));
    charData.setProperties(QLowEnergyCharacteristic::Read | QLowEnergyCharacteristic::Notify | QLowEnergyCharacteristic::Write);

    QLowEnergyServiceData serviceData;
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(QBluetoothUuid(QString("ad100000-d901-11e8-9f8b-f2801f1b9fd1")));
    serviceData.addCharacteristic(charData);

    const QScopedPointer<QLowEnergyController> peripheral(QLowEnergyController::createPeripheral());
    const QScopedPointer<QLowEnergyService> service(peripheral->addService(serviceData));
    peripheral->startAdvertising(QLowEnergyAdvertisingParameters(), advertisingData, advertisingData);

    auto reconnect = [&peripheral, advertisingData]() {
        peripheral->startAdvertising(QLowEnergyAdvertisingParameters(), advertisingData, advertisingData);
    };
    QObject::connect(peripheral.data(), &QLowEnergyController::disconnected, reconnect);

    return app.exec();
}

/*
int main(int argc, char *argv[])
{
    //QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth* = true"));
    QCoreApplication app(argc, argv);

    QLowEnergyAdvertisingData advertisingData;
    advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    advertisingData.setIncludePowerLevel(true);
    advertisingData.setLocalName("HeartRateServer");
    advertisingData.setServices(QList<QBluetoothUuid>() << QBluetoothUuid::HeartRate);

    QLowEnergyCharacteristicData charData;
    charData.setUuid(QBluetoothUuid::HeartRateMeasurement);
    charData.setValue(QByteArray(2, 0));
    charData.setProperties(QLowEnergyCharacteristic::Notify);
    const QLowEnergyDescriptorData clientConfig(QBluetoothUuid::ClientCharacteristicConfiguration,
                                                QByteArray(2, 0));
    charData.addDescriptor(clientConfig);

    QLowEnergyServiceData serviceData;
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(QBluetoothUuid::HeartRate);
    serviceData.addCharacteristic(charData);

    const QScopedPointer<QLowEnergyController> leController(QLowEnergyController::createPeripheral());
    const QScopedPointer<QLowEnergyService> service(leController->addService(serviceData));
    leController->startAdvertising(QLowEnergyAdvertisingParameters(), advertisingData,
                                   advertisingData);

    QTimer heartbeatTimer;
    quint8 currentHeartRate = 60;
    enum ValueChange { ValueUp, ValueDown } valueChange = ValueUp;
    const auto heartbeatProvider = [&service, &currentHeartRate, &valueChange]() {
        QByteArray value;
        value.append(char(0)); // Flags that specify the format of the value.
        value.append(char(currentHeartRate)); // Actual value.
        QLowEnergyCharacteristic characteristic
                = service->characteristic(QBluetoothUuid::HeartRateMeasurement);
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

    auto reconnect = [&leController, advertisingData]() {
        leController->startAdvertising(QLowEnergyAdvertisingParameters(), advertisingData,
                                       advertisingData);
    };
    QObject::connect(leController.data(), &QLowEnergyController::disconnected, reconnect);

    return app.exec();
}
*/
