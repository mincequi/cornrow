#include "Adapter.h"

#include "Central.h"
#include "Converter.h"
#include "Defines.h"
#include "Peripheral.h"

#include <QtBluetooth/QLowEnergyCharacteristic>

namespace ble
{

Adapter::Adapter(Central* central)
    : QObject(central),
      m_central(central),
      m_converter(new Converter())
{
    connect(m_central, &Central::characteristicRead, this, &Adapter::onCharacteristicChanged);
}

Adapter::Adapter(Peripheral* peripheral, const std::vector<common::Filter>& filters)
    : QObject(peripheral),
      m_peripheral(peripheral),
      m_converter(new Converter())
{
    m_peripheral->setCharacteristics( {{ peqCharacteristicUuid, m_converter->filtersToBle(filters)}} );
    connect(m_peripheral, &Peripheral::characteristicChanged, this, &Adapter::onCharacteristicChanged);
}

Adapter::~Adapter()
{
    delete m_converter;
}

void Adapter::onCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
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
