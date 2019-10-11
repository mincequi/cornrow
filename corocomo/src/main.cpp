#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSurfaceFormat>

#include "BusyIndicatorModel.h"
#include "EqChart.h"
#include "Model.h"
#include "Config.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QSurfaceFormat format;
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);

    QGuiApplication app(argc, argv);

    auto config = Config::init(Config::Type::High);
    auto model = Model::init(*config);
    Q_UNUSED(model)

    qmlRegisterType<BusyIndicatorModel>("Cornrow.BusyIndicatorModel", 1, 0, "CornrowBusyIndicatorModel");
    qmlRegisterType<EqChart>("Cornrow.EqChart", 1, 0, "CornrowEqChart");
    qmlRegisterSingletonType<Config>("Cornrow.Configuration", 1, 0, "CornrowConfiguration",
                                    [](QQmlEngine*, QJSEngine*) -> QObject* {
        return Config::instance();
    });
    qmlRegisterSingletonType<Model>("Cornrow.Model", 1, 0, "CornrowModel",
                                    [](QQmlEngine*, QJSEngine*) -> QObject* {
        return Model::instance();
    });

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
