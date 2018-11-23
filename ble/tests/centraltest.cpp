#include <QCoreApplication>
#include <QDebug>
#include <QtBluetooth/QLowEnergyCharacteristic>

#include <ble/Client.h>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    auto central = new ble::Client();
    QObject::connect(central, &ble::Client::characteristicRead, [](common::FilterTask task, const QByteArray &value) {
        qDebug() << "task:" << (int)task << ", value:" << value.toHex();
    });
    central->startDiscovering();

    return app.exec();
}
