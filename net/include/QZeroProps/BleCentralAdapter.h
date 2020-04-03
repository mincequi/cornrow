#pragma once

#include <QObject>
#include <QTimer>

#include "QZeroPropsBluetoothLeService.h"
#include <common/Types.h>
#include <common/ble/Types.h>

//#include "DeviceModel.h"

#include <QZeroProps/QZeroPropsService.h>

class IoModel;

class BleCentralAdapter : public QObject
{
    Q_OBJECT

public:
    explicit BleCentralAdapter(QZeroProps::QZeroPropsBluetoothLeService* central);
    ~BleCentralAdapter();
    
    // @TODO(mawe): HACK
    QZeroProps::QZeroPropsBluetoothLeService* central() { return m_central; }

    void connectDevice(QZeroProps::QZeroPropsService* service);

    void setDirty(common::ble::CharacteristicType group);
    void setDirty(const std::string& uuid);

	// @TODO(mawe): think about how to inject models in adapters/services
    void setIoModel(IoModel* ioModel);

signals:
    void status(QZeroProps::QZeroPropsClient::State state, const QString& errorString = QString());
    void presetReceived(uint8_t index, uint8_t total, uint8_t active, const QString& name);

private:
    void doWriteCharc();
    void onStatus(QZeroProps::QZeroPropsClient::State state, const QString& errorString);

    QZeroProps::QZeroPropsBluetoothLeService* m_central;
    IoModel*        m_ioModel;

    QTimer          m_timer;

    Q_DECLARE_FLAGS(FilterGroups, common::ble::CharacteristicType)
    FilterGroups m_dirtyFilterGroups;
    std::set<std::string> m_dirtyCharcs;
};


