#include <QCoreApplication>
#include <QDebug>
#include <QtBluetooth/QLowEnergyCharacteristic>

#include <ble/Central.h>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    auto central = new ble::Central();
    QObject::connect(central, &ble::Central::characteristicRead, [](common::FilterTask task, const QByteArray &value) {
        qDebug() << "task:" << (int)task << ", value:" << value.toHex(',');
    });
    central->startDiscovering();

    return app.exec();
}
