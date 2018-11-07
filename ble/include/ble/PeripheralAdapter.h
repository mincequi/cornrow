#pragma once

#include <QObject>

#include <common/Types.h>

class QLowEnergyCharacteristic;

namespace ble
{
class Converter;
class Peripheral;

class PeripheralAdapter : public QObject
{
    Q_OBJECT

public:
    explicit PeripheralAdapter(Peripheral* peripheral, const std::vector<common::Filter>& filters);
    ~PeripheralAdapter();

signals:
    void peq(const QVector<common::Filter>& filters);
    void crossover(const QByteArray& value);
    void loudness(const QByteArray& value);

private:
    void onCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);

    Peripheral* m_peripheral;
    Converter* m_converter;
};

} // namespace ble
