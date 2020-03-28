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
    
    void startDiscovering();
    
    void connectDevice(const QBluetoothDeviceInfo& device);

    void setDirty(common::ble::CharacteristicType group);
    void setDirty(const std::string& uuid);

	// @TODO(mawe): think about how to inject models in adapters/services
    void setIoModel(IoModel* ioModel);

signals:
	void deviceDiscovered(const QBluetoothDeviceInfo& device);

    void status(DeviceModel::Status status, const QString& errorString = QString());
    void ioCapsReceived(const std::vector<common::IoInterface>& inputs, const std::vector<common::IoInterface>& ouputs);
    void ioConfReceived(common::IoInterface input, common::IoInterface ouput);
    void presetReceived(uint8_t index, uint8_t total, uint8_t active, const QString& name);

private:
    void doWriteCharc();
    void onStatus(ble::BleClient::Status status, const QString& errorString);
    void onCharacteristicChanged(const std::string& uuid, const QByteArray& value);

    ble::BleClient* m_central;
    IoModel*        m_ioModel;

    QTimer          m_timer;

    Q_DECLARE_FLAGS(FilterGroups, common::ble::CharacteristicType)
    FilterGroups m_dirtyFilterGroups;
    std::set<std::string> m_dirtyCharcs;
};


