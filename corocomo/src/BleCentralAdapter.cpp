#include "BleCentralAdapter.h"

#include <ble/Central.h>

#include "Model.h"

BleCentralAdapter::BleCentralAdapter(ble::Central* central)
    : QObject(central),
      m_central(central)
{
    connect(m_central, &ble::Central::characteristicRead, this, &BleCentralAdapter::onCharacteristicRead);
    connect(m_central, &ble::Central::status, this, &BleCentralAdapter::onStatus);
}

BleCentralAdapter::~BleCentralAdapter()
{
}

void BleCentralAdapter::setFilters(const std::vector<Model::Filter>& filters)
{
    QByteArray value(filters.size()*4, 0);

    for (uint i = 0; i < filters.size(); ++i) {
        value[i*4]   = static_cast<char>(filters.at(i).t);
        value[i*4+1] = filters.at(i).f;
        value[i*4+2] = filters.at(i).g*2.0;
        value[i*4+3] = filters.at(i).q;
    }

    m_central->writeCharacteristic(common::FilterTask::Peq, value);
}

void BleCentralAdapter::onStatus(ble::Central::Status _status, const QString& errorString)
{
    switch (_status) {
    case ble::Central::Status::Discovering:
        emit status(Model::Status::Discovering);
        return;
    case ble::Central::Status::Connected:
        emit status(Model::Status::Connected);
        return;
    case ble::Central::Status::Timeout:
        emit status(Model::Status::Timeout);
        return;
    case ble::Central::Status::Lost:
        emit status(Model::Status::Lost);
        return;
    case ble::Central::Status::Error:
        emit status(Model::Status::Error, errorString);
        return;
    }
}

void BleCentralAdapter::onCharacteristicRead(common::FilterTask task, const QByteArray &value)
{
    switch (task) {
    case common::FilterTask::Peq: {
        if (value.size()%4 != 0) {
            return;
        }
        std::vector<Model::Filter> filters;
        filters.reserve(value.size()/4);
        for (int i = 0; i < value.size(); i += 4) {
            filters.push_back({ static_cast<common::FilterType>(value.at(i)),
                                static_cast<uint8_t>(value.at(i+1)),
                                static_cast<int8_t>(value.at(i+2))/2.0f,
                                static_cast<uint8_t>(value.at(i+3)) });
        }
        emit peq(filters);

        break;
    }
    case common::FilterTask::Crossover:
    case common::FilterTask::Loudness:
    case common::FilterTask::Invalid:
        break;
    }
}
