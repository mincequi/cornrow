#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <ble/Central.h>

#include "BleCentralAdapter.h"
#include "EqChart.h"
#include "Model.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterType<EqChart>("Cornrow.EqChart", 1, 0, "CornrowEqChart");
    qmlRegisterSingletonType<Model>("Cornrow.Model", 1, 0, "CornrowModel",
                                    [](QQmlEngine*, QJSEngine*) -> QObject* {
        return Model::instance();
    });

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    // BLE
    auto central = new ble::Central();
    auto adapter = new ble::CentralAdapter(central);
    QObject::connect(adapter, &ble::CentralAdapter::peq, Model::instance(), &Model::setFilters);

    return app.exec();
}
