#pragma once

#include <functional>

#include <QObject>

#include <common/Types.h>

class QBluetoothUuid;
class QLowEnergyCharacteristic;

namespace ble
{
class Converter;
class Server;

class ServerAdapter : public QObject
{
    Q_OBJECT

public:
    using ValuesProvider = std::function<std::vector<common::Filter>()>;
    explicit ServerAdapter(Server* server, ValuesProvider valuesProvider);
    ~ServerAdapter();

signals:
    void peq(const std::vector<common::Filter>& filters);
    void aux(const QByteArray& value);

private:
    void onCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    std::map<QBluetoothUuid, QByteArray> provideCharcs();

    Server* m_server;
    ValuesProvider m_valuesProvider;
    Converter* m_converter;
};

} // namespace ble
