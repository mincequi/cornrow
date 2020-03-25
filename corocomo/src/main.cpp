#include "BleCentralAdapter.h"
#include "BodePlotModel.h"
#include "BusyIndicatorModel.h"
#include "Config.h"
#include "DeviceModel.h"
#include "EqChart.h"
#include "FilterModel.h"
#include "IoModel.h"
#include "PhaseChart.h"
#include "PresetModel.h"
#include "SoftClipChart.h"
#include "ble/BleClient.h"
#include "net/NetClient.h"
#include <common/RemoteDataStore.h>

#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QSurfaceFormat format;
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);

    QGuiApplication app(argc, argv);
    
    // Remote services: BLE, Tcp
    common::RemoteDataStore remoteDataStore(nullptr);
    ble::BleClient bleClient;
    BleCentralAdapter bleAdapter(&bleClient);
    net::TcpClient netClient(&remoteDataStore);

    // Setup view models
    auto config = Config::init(Config::Type::Low);
    DeviceModel::init(&bleAdapter, &netClient);
    auto ioModel = IoModel::init(&bleAdapter);
    // @TODO(mawe): remove bleAdapter from FilterModel
    auto filterModel = FilterModel::init(*config, &bleAdapter, &remoteDataStore);
    BodePlotModel::init(*config);
    PresetModel::init(&bleAdapter);
    
    bleAdapter.setModel(filterModel);
    bleAdapter.setIoModel(ioModel);

    QObject::connect(&remoteDataStore, &common::RemoteDataStore::peqChanged, [&]() {
        netClient.setProperty("peq", remoteDataStore.peq());
    });
    QObject::connect(&remoteDataStore, &common::RemoteDataStore::auxChanged, [&]() {
        netClient.setProperty("aux", remoteDataStore.aux());
    });

    qmlRegisterUncreatableMetaObject(net::NetDevice::staticMetaObject, "Cornrow.DeviceType", 1, 0, "CornrowDeviceType", "Only enums");

    qmlRegisterType<BusyIndicatorModel>("Cornrow.BusyIndicatorModel", 1, 0, "CornrowBusyIndicatorModel");
    qmlRegisterType<EqChart>("Cornrow.EqChart", 1, 0, "CornrowEqChart");
    qmlRegisterType<PhaseChart>("Cornrow.PhaseChart", 1, 0, "CornrowPhaseChart");
    qmlRegisterType<SoftClipChart>("Cornrow.SoftClipChart", 1, 0, "CornrowSoftClipChart");

    qmlRegisterSingletonType<Config>("Cornrow.Configuration", 1, 0, "CornrowConfiguration", [](QQmlEngine*, QJSEngine*) -> QObject* {
        return Config::instance();
    });
    /*
    qmlRegisterSingletonType<BusyIndicatorModel>("Cornrow.BusyIndicatorModel", 1, 0, "CornrowBusyIndicatorModel", [](QQmlEngine*, QJSEngine*) -> QObject* {
        return BusyIndicatorModel::instance();
    });
    */
    qmlRegisterSingletonType<DeviceModel>("Cornrow.DeviceModel", 1, 0, "DeviceModel", [](QQmlEngine*, QJSEngine*) -> QObject* {
        return DeviceModel::instance();
    });
    qmlRegisterSingletonType<FilterModel>("Cornrow.FilterModel", 1, 0, "FilterModel", [](QQmlEngine*, QJSEngine*) -> QObject* {
        return FilterModel::instance();
    });
    qmlRegisterSingletonType<BodePlotModel>("Cornrow.BodePlotModel", 1, 0, "CornrowBodePlotModel", [](QQmlEngine*, QJSEngine*) -> QObject* {
        return BodePlotModel::instance();
    });
    qmlRegisterSingletonType<IoModel>("Cornrow.IoModel", 1, 0, "CornrowIoModel", [](QQmlEngine*, QJSEngine*) -> QObject* {
        return IoModel::instance();
    });
    qmlRegisterSingletonType<PresetModel>("Cornrow.PresetModel", 1, 0, "CornrowPresetModel", [](QQmlEngine*, QJSEngine*) -> QObject* {
        return PresetModel::instance();
    });

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/src/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
