#include <QCoreApplication>
#include <QDebug>

#include <ble/Server.h>
#include <ble/ServerAdapter.h>

void printFilters(const std::vector<common::Filter>& filters)
{
    for (const auto& filter : filters) {
        qDebug() << "t:" << static_cast<uint>(filter.type)
                 << "f:" << filter.f
                 << "g:" << filter.g
                 << "q:" << filter.q;
    }
    qDebug() << "";
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    auto peripheral = new ble::Server();
    auto adapter = new ble::ServerAdapter(peripheral, []() -> std::vector<common::Filter>
    {
        return { { common::FilterType::Peak, 2335.0, -9.7, 15.7 },
                 { common::FilterType::LowPass, 12222.0, 3.4, 0.5 },
                 { common::FilterType::HighPass, 47.0, -8.3, 0.8 } };
    });

    QObject::connect(adapter, &ble::ServerAdapter::peq, &printFilters);

    return app.exec();
}
