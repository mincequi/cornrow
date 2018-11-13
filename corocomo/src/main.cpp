#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "EqChart.h"
#include "Model.h"
#include "ModelConfiguration.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    auto modelConfiguration = ModelConfiguration::init(ModelConfiguration::Type::Mid);
    auto model = Model::init(*modelConfiguration);
    Q_UNUSED(model)

    qmlRegisterType<EqChart>("Cornrow.EqChart", 1, 0, "CornrowEqChart");
    qmlRegisterSingletonType<ModelConfiguration>("Cornrow.Configuration", 1, 0, "CornrowConfiguration",
                                    [](QQmlEngine*, QJSEngine*) -> QObject* {
        return ModelConfiguration::instance();
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
