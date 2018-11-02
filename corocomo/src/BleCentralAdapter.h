#pragma once

#include <QObject>

#include <common/Types.h>

class QLowEnergyCharacteristic;

namespace ble
{
class Central;

class CentralAdapter : public QObject
{
    Q_OBJECT

public:
    explicit CentralAdapter(Central* central);
    ~CentralAdapter();

signals:
    void peq(const std::vector<common::Filter>& filters);
    void crossover(const QByteArray& value);
    void loudness(const QByteArray& value);

private:
    void onCharacteristicRead(common::FilterTask task, const QByteArray &value);

    Central* m_central;
};

} // namespace ble
