#include <QCoreApplication>

#include <ble/Peripheral.h>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    auto peripheral = new ble::Peripheral();

    return app.exec();
}
