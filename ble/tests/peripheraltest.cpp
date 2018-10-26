#include <QCoreApplication>

#include <ble/Controller.h>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    auto bleController = new ble::Controller();

    return app.exec();
}
