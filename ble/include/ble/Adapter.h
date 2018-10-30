#pragma once

#include <QObject>

#include <common/Types.h>

class QLowEnergyCharacteristic;

namespace ble
{
class Central;
class Converter;
class Peripheral;

class Adapter : public QObject
{
    Q_OBJECT

public:
    explicit Adapter(Central* central);
    explicit Adapter(Peripheral* peripheral, const std::vector<common::Filter>& filters);
    ~Adapter();

signals:
    void peq(const std::vector<common::Filter>& filters);
    void crossover(const QByteArray& value);
    void loudness(const QByteArray& value);

private:
    void onCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);

    Central* m_central;
    Peripheral* m_peripheral;

    Converter* m_converter;
};

} // namespace ble
