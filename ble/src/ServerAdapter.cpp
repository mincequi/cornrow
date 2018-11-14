#include "ServerAdapter.h"

#include "Converter.h"
#include "Defines.h"
#include "Server.h"

#include <QTimer>
#include <QtBluetooth/QLowEnergyCharacteristic>

namespace ble
{

ServerAdapter::ServerAdapter(Server* server, ValuesProvider valuesProvides)
    : QObject(server),
      m_server(server),
      m_valuesProvider(valuesProvides),
      m_converter(new Converter())
{
    m_server->init(std::bind(&ServerAdapter::provideCharcs, this));
    connect(m_server, &Server::characteristicChanged, this, &ServerAdapter::onCharacteristicChanged);
}

ServerAdapter::~ServerAdapter()
{
    delete m_converter;
}

void ServerAdapter::onCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
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

std::map<QBluetoothUuid, QByteArray> ServerAdapter::provideCharcs()
{
    return {{ peqCharacteristicUuid, m_converter->filtersToBle(m_valuesProvider())}};
}

} // namespace ble
