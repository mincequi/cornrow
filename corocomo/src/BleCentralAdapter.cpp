#include "BleCentralAdapter.h"

#include <ble/BleClient.h>

#include "IoModel.h"
#include "FilterModel.h"

#include <QUuid>

BleCentralAdapter::BleCentralAdapter(ble::BleClient* central)
    : QObject(central),
      m_central(central)
{
    connect(m_central, &ble::BleClient::status, this, &BleCentralAdapter::onStatus);

    m_timer.setInterval(200);
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, &BleCentralAdapter::doWriteCharc);
}

BleCentralAdapter::~BleCentralAdapter()
{
}

void BleCentralAdapter::startDiscovering(const QUuid& serviceUuid)
{
    m_central->startDiscovering(serviceUuid);
}

void BleCentralAdapter::connectDevice(const QBluetoothDeviceInfo& device)
{
    m_central->connectDevice(device);
}

void BleCentralAdapter::setDirty(common::ble::CharacteristicType group)
{
    // @TODO(mawe): remove FilterGroup type
    m_dirtyFilterGroups.setFlag(group);

    if (!m_timer.isActive()) {
        m_timer.start();
    }
}

void BleCentralAdapter::setDirty(const std::string& uuid)
{
    m_dirtyCharcs.insert(uuid);

    if (!m_timer.isActive()) {
        m_timer.start();
    }
}

void BleCentralAdapter::setIoModel(IoModel* ioModel)
{
    m_ioModel = ioModel;
}

void BleCentralAdapter::doWriteCharc()
{
    if (m_dirtyCharcs.count(common::ble::ioConfCharacteristicUuid)) {
        QByteArray value(2, 0);
        auto i = m_ioModel->input();
        if (i.number > 0) i.number -= 1; // Correct number to index
        auto o = m_ioModel->output();
        if (o.number > 0) o.number -= 1; // Correct number to index
        value[0] = *reinterpret_cast<char*>(&i);
        value[1] = *reinterpret_cast<char*>(&o);
        m_central->setCharacteristic(common::ble::ioConfCharacteristicUuid, value);
        m_dirtyCharcs.erase(common::ble::ioConfCharacteristicUuid);
    }
}

void BleCentralAdapter::onStatus(ble::BleClient::Status _status, const QString& statusText)
{
    switch (_status) {
    case ble::BleClient::Status::NoBluetooth:
        emit status(DeviceModel::Status::NoBluetooth);
        return;
    case ble::BleClient::Status::Discovering:
        emit status(DeviceModel::Status::Discovering);
        return;
    case ble::BleClient::Status::Connecting:
        emit status(DeviceModel::Status::Connecting, statusText);
        return;
    case ble::BleClient::Status::Connected:
        emit status(DeviceModel::Status::Connected);
        return;
    case ble::BleClient::Status::Timeout:
        emit status(DeviceModel::Status::Idle);
        return;
    case ble::BleClient::Status::Lost:
        emit status(DeviceModel::Status::Lost);
        return;
    case ble::BleClient::Status::Error:
        emit status(DeviceModel::Status::Error, statusText);
        return;
    }
}
