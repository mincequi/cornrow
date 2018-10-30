#include <QCoreApplication>
#include <QDebug>
#include <QtBluetooth/QLowEnergyCharacteristic>

#include <ble/Central.h>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    auto central = new ble::Central();
    QObject::connect(central, &ble::Central::characteristicRead, [](const QLowEnergyCharacteristic &info, const QByteArray &value) {
        qDebug() << "info:" << info.uuid() << ", value:" << value;
    });
    central->startDiscovering();

    return app.exec();
}
