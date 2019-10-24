#include <QCoreApplication>
#include <QDebug>
#include <QtBluetooth/QLowEnergyCharacteristic>

#include <ble/Client.h>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    auto central = new ble::Client();
    QObject::connect(central, &ble::Client::characteristicRead, [](const std::string& charc, const QByteArray &value) {
        qDebug() << "charc:" << QString::fromStdString(charc) << ", value:" << value.toHex();
    });
    central->startDiscovering();

    return app.exec();
}
