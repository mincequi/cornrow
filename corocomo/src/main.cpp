#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "EqChart.h"
#include "Model.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    ModelConfiguration lowConf;
    lowConf.filterCount = 5;  // @TODO(mawe): implement dynamic filter count
    lowConf.freqTable = common::frequencyTable;
    lowConf.freqDefault = 144;
    lowConf.freqMin = 8;
    lowConf.freqMax = 248;
    lowConf.freqStep = 8;
    lowConf.gainMin = -24.0;
    lowConf.gainMax = 6.0;
    lowConf.gainStep = 1.0;
    lowConf.qTable = common::qTable;
    lowConf.qDefault = 17;
    lowConf.qMin = 14;
    lowConf.qMax = 34;
    lowConf.qStep = 1;

    ModelConfiguration midConf = lowConf;
    midConf.freqStep = 2;
    midConf.gainStep = 0.5;
    midConf.qMin = 0;
    midConf.qMax = midConf.qTable.size()-1;

    auto model = Model::init(midConf);
    Q_UNUSED(model)

    qmlRegisterType<EqChart>("Cornrow.EqChart", 1, 0, "CornrowEqChart");
    qmlRegisterUncreatableType<ModelConfiguration>("Cornrow.ModelConfiguration", 1, 0, "CornrowModelConfiguration", "Model is const");
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
