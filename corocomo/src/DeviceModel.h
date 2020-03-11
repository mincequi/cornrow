#pragma once

#include <QObject>
#include <common/Types.h>
#include <net/NetClient.h>

class BleCentralAdapter;
namespace ble {
class Client;
}

class QZeroConf;
class QZeroConfServiceData;
typedef QSharedPointer<QZeroConfServiceData> QZeroConfService;

class DeviceModel : public QObject
{
	Q_OBJECT

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusLabel READ statusLabel NOTIFY statusChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)

    Q_PROPERTY(std::vector<net::NetDevice> devices READ devices NOTIFY devicesChanged)

public:
    enum Status : uint8_t {
        NoBluetooth,
        Discovering,
        Connecting,
        Connected,
        Timeout,
        Lost,
        Error
    };
    Q_ENUM(Status)

    static DeviceModel* instance();

    Q_INVOKABLE void startDiscovering();

    Status      status() const;
    QString     statusLabel() const;
    QString     statusText() const;

    std::vector<net::NetDevice> devices() const;

signals:
    void statusChanged();
    void devicesChanged();

private:
	explicit DeviceModel(QObject *parent = nullptr);

    void onAppStateChanged(Qt::ApplicationState state);
    void onNetDeviceDiscovered(const net::NetDevice& device);

    static DeviceModel* s_instance;

    Status          m_status = Status::Discovering;
    QString         m_statusLabel = "Discovering";
    QString         m_statusText;
    bool            m_demoMode = false;

    std::vector<net::NetDevice> m_devices;

    // BLE
    ble::Client* m_bleClient = nullptr;
    BleCentralAdapter* m_bleAdapter = nullptr;
    friend class BleCentralAdapter;

    // Net
    net::NetClient* m_netClient = nullptr;
    QZeroConf* m_zeroConf = nullptr;
};
