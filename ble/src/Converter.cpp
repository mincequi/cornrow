#include "Converter.h"

#include <math.h>

#include <QByteArray>

#include "Defines.h"

namespace ble
{

Converter::Converter()
{
}

Converter::~Converter()
{
}

common::FilterGroup Converter::fromBle(const QBluetoothUuid& uuid)
{
    if (uuid == common::peqCharacteristicUuid) {
        return common::FilterGroup::Peq;
    } else if (uuid == common::auxCharacteristicUuid) {
        return common::FilterGroup::Aux;
    }

    return common::FilterGroup::Invalid;
}

QBluetoothUuid Converter::toBle(common::FilterGroup group)
{
    switch (group) {
    case common::FilterGroup::Peq:
        return common::peqCharacteristicUuid;
    case common::FilterGroup::Aux:
        return common::auxCharacteristicUuid;
    case common::FilterGroup::Invalid:
        return QBluetoothUuid();
    }

    return QBluetoothUuid();
}

QByteArray Converter::filtersToBle(const std::vector<common::Filter>& filters)
{
    QByteArray value;
    value.reserve(filters.size()*4);

    for (const auto& filter : filters) {
        value.append(static_cast<char>(filter.type));
        value.append(fToBle(filter.f));
        value.append(gToBle(filter.g));
        value.append(qToBle(filter.q));
    }

    return value;
}

std::vector<common::Filter> Converter::filtersFromBle(const QByteArray& array)
{
    if (array.size()%4 != 0) {
        return {};
    }

    std::vector<common::Filter> filters;
    filters.reserve(array.size()/4);
    for (int i = 0; i < array.size(); i+=4) {
        filters.push_back( { static_cast<common::FilterType>(array.at(i)),
                             fFromBle(array.at(i+1)),
                             gFromBle(array.at(i+2)),
                             qFromBle(array.at(i+3)) } );
    }
    return filters;
}

uint8_t Converter::fToBle(float f)
{
    uint8_t i = 0;
    for (; i < common::frequencyTable.size(); ++i) {
        if (common::frequencyTable.at(i+1) >= f) break;
    }

    float c = sqrt(common::frequencyTable.at(i)*common::frequencyTable.at(i+1));
    if (f <= c) return i;
    else return i+1;
}

float Converter::fFromBle(uint8_t i)
{
    if (i >= common::frequencyTable.size()) return 0.0;
    return common::frequencyTable[i];
}

int8_t Converter::gToBle(float g)
{
    return round(g * 2.0);
}

float Converter::gFromBle(int8_t g)
{
    return g * 0.5;
}

uint8_t Converter::qToBle(float f)
{
    uint8_t i = 0;
    for (; i < common::qTable.size(); ++i) {
        if (common::qTable.at(i+1) >= f) break;
    }

    float c = sqrt(common::qTable.at(i)*common::qTable.at(i+1));
    if (f <= c) return i;
    else return i+1;
}

float Converter::qFromBle(uint8_t i)
{
    if (i >= common::qTable.size()) return 0.0;
    return common::qTable[i];
}

} // namespace v1
