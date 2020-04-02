#pragma once

#include <QObject>
#include <QTimer>

#include <ble/BleClient.h>
#include <common/Types.h>

#include "DeviceModel.h"

class IoModel;

class BleCentralAdapter : public QObject
{
    Q_OBJECT

public:
    explicit BleCentralAdapter(ble::BleClient* central);
    ~BleCentralAdapter();
    
    // @TODO(mawe): HACK
    ble::BleClient* central() { return m_central; }

    void startDiscovering(const QUuid& serviceUuid);
    
    void connectDevice(const QBluetoothDeviceInfo& device);

    void setDirty(common::ble::CharacteristicType group);
    void setDirty(const std::string& uuid);

	// @TODO(mawe): think about how to inject models in adapters/services
    void setIoModel(IoModel* ioModel);

signals:
    void status(DeviceModel::Status status, const QString& errorString = QString());
    void presetReceived(uint8_t index, uint8_t total, uint8_t active, const QString& name);

private:
    void doWriteCharc();
    void onStatus(ble::BleClient::Status status, const QString& errorString);

    ble::BleClient* m_central;
    IoModel*        m_ioModel;

    QTimer          m_timer;

    Q_DECLARE_FLAGS(FilterGroups, common::ble::CharacteristicType)
    FilterGroups m_dirtyFilterGroups;
    std::set<std::string> m_dirtyCharcs;
};


