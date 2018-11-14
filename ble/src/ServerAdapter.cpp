#include "ServerAdapter.h"

#include "Converter.h"
#include "Defines.h"
#include "Server.h"

#include <QTimer>
#include <QtBluetooth/QLowEnergyCharacteristic>

namespace ble
{

PeripheralAdapter::PeripheralAdapter(Peripheral* peripheral, const std::vector<common::Filter>& filters)
    : QObject(peripheral),
      m_peripheral(peripheral),
      m_converter(new Converter())
{
    m_peripheral->init( {{ peqCharacteristicUuid, m_converter->filtersToBle(filters)}} );
    connect(m_peripheral, &Peripheral::characteristicChanged, this, &PeripheralAdapter::onCharacteristicChanged);
}

PeripheralAdapter::~PeripheralAdapter()
{
    delete m_converter;
}

void PeripheralAdapter::onCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    // ignore any other characteristic, should not happen
    if (characteristic.uuid() == ble::peqCharacteristicUuid) {
        emit peq(m_converter->filtersFromBle(value));
    } else if (characteristic.uuid() == ble::crossoverCharacteristicUuid) {
        emit crossover(value);
    } else if (characteristic.uuid() == ble::loudnessCharacteristicUuid) {
        emit loudness(value);
    } else {
        qDebug() << __func__ << "unknown uuid:" << characteristic.uuid();
    }
}

} // namespace ble
