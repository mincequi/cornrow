#include <QZeroProps/BleCentralAdapter.h>

#include "../../../corocomo/src/IoModel.h"
#include "../../../corocomo/src/FilterModel.h"

#include <QUuid>

BleCentralAdapter::BleCentralAdapter(QZeroProps::QZeroPropsBluetoothLeService* central)
    : QObject(central),
      m_central(central)
{
    connect(m_central, &QZeroProps::QZeroPropsBluetoothLeService::status, this, &BleCentralAdapter::onStatus);

    m_timer.setInterval(200);
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, &BleCentralAdapter::doWriteCharc);
}

BleCentralAdapter::~BleCentralAdapter()
{
}

void BleCentralAdapter::connectDevice(QZeroProps::QZeroPropsService* service)
{
    m_central->connectToService(service);
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

void BleCentralAdapter::onStatus(QZeroProps::QZeroPropsClient::State _status, const QString& statusText)
{
    emit status(_status, statusText);
}
