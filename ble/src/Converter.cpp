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

common::FilterTask Converter::fromBle(const QBluetoothUuid& uuid)
{
    if (uuid == ble::peqCharacteristicUuid) {
        return common::FilterTask::Peq;
    } else if (uuid == ble::crossoverCharacteristicUuid) {
        return common::FilterTask::Crossover;
    } else if (uuid == ble::loudnessCharacteristicUuid) {
        return common::FilterTask::Loudness;
    }

    return common::FilterTask::Invalid;
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
    for (; i < common::twelfthOctaveBandsTable.size(); ++i) {
        if (common::twelfthOctaveBandsTable.at(i+1) >= f) break;
    }

    float c = sqrt(common::twelfthOctaveBandsTable.at(i)*common::twelfthOctaveBandsTable.at(i+1));
    if (f <= c) return i;
    else return i+1;
}

float Converter::fFromBle(uint8_t i)
{
    if (i >= common::twelfthOctaveBandsTable.size()) return 0.0;
    return common::twelfthOctaveBandsTable[i];
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
