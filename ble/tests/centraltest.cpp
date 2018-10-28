#include <QCoreApplication>
#include <QDebug>

#include <ble/Central.h>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    auto central = new ble::Central();
    QObject::connect(central, &ble::Central::peq, [](const QByteArray& value) {
        qDebug() << "peq:" << value;
    });
    central->startDiscovering();

    return app.exec();
}
