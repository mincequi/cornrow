#pragma once

#include <QObject>
#include <common/Types.h>
#include <QZeroProps/QZeroPropsClient.h>

class BleCentralAdapter;
namespace ble {
class BleClient;
}

class QBluetoothDeviceInfo;
class QZeroConf;
class QZeroConfServiceData;
typedef QSharedPointer<QZeroConfServiceData> QZeroConfService;

class DeviceModel : public QObject
{
	Q_OBJECT

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusLabel READ statusLabel NOTIFY statusChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)

    // @TODO(Qt): QObjectList is not accepted. So, we must use QList<QObject*>.
    Q_PROPERTY(QList<QObject*> devices READ devices NOTIFY devicesChanged)

public:
    enum Status : uint8_t {
        NoBluetooth,
        Discovering,
        Connecting,
        Connected,
        Idle,
        Lost,
        Error
    };
    Q_ENUM(Status)

    static DeviceModel* init(BleCentralAdapter* bleClient, QZeroProps::QZeroPropsClient* netClient);
    static DeviceModel* instance();

    Q_INVOKABLE void startDiscovering();
	Q_INVOKABLE void startDemo();

    Status      status() const;
    QString     statusLabel() const;
    QString     statusText() const;

    QObjectList devices() const;
    Q_INVOKABLE void connectToService(QZeroProps::QZeroPropsService* device);

signals:
    void statusChanged();
    void devicesChanged();

private:
    explicit DeviceModel(BleCentralAdapter* bleAdapter, QZeroProps::QZeroPropsClient* m_tcpClient, QObject* parent = nullptr);
    
    void stopDiscovering();

    void onAppStateChanged(Qt::ApplicationState state);
    void onBleDeviceStatus(Status status, const QString& errorString = QString());
    void onNetDeviceStatus(QZeroProps::QZeroPropsClient::State state, const QString& errorString);
    void onBleDeviceDiscovered(const QBluetoothDeviceInfo& device);
    void onNetDeviceDiscovered(QZeroProps::QZeroPropsServicePtr device);
    void onNetDeviceDisappeared(const QHostAddress& address);

    static DeviceModel* s_instance;

    Status          m_status = Status::Discovering;
    QString         m_statusLabel = "Discovering";
    QString         m_statusText;
    bool            m_demoMode = false;

    QList<QZeroProps::QZeroPropsServicePtr> m_devices;

    // BLE
    BleCentralAdapter* m_bleAdapter = nullptr;
    friend class BleCentralAdapter;

    // Net
    QZeroProps::QZeroPropsClient* m_tcpClient = nullptr;
};
