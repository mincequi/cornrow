#include <QCoreApplication>

#include <ble/Peripheral.h>
#include <ble/PeripheralAdapter.h>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    auto peripheral = new ble::Peripheral();
    new ble::PeripheralAdapter(peripheral, {
        { common::FilterType::Peak, 2335.0, -9.7, 15.7 },
        { common::FilterType::LowPass, 12222.0, 3.4, 0.5 },
        { common::FilterType::HighPass, 47.0, -8.3, 0.8 }
    });

    return app.exec();
}
