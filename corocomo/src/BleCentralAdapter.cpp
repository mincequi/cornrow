#include "BleCentralAdapter.h"

#include <ble/Central.h>

#include "Model.h"

BleCentralAdapter::BleCentralAdapter(ble::Central* central, Model* model)
    : QObject(central),
      m_central(central),
      m_model(model)
{
    connect(m_central, &ble::Central::characteristicRead, this, &BleCentralAdapter::onCharacteristicRead);
    connect(m_central, &ble::Central::status, this, &BleCentralAdapter::onStatus);

    m_timer.setInterval(100);
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, &BleCentralAdapter::doWriteCharc);
}

BleCentralAdapter::~BleCentralAdapter()
{
}

void BleCentralAdapter::setPeqDirty()
{
    if (!m_timer.isActive()) {
        m_timer.start();
    }
}

void BleCentralAdapter::doWriteCharc()
{
    const auto& filters = m_model->m_filters;
    const auto& config = m_model->m_configuration;
    QByteArray value(filters.size()*4, 0);

    for (int i = 0; i < filters.size(); ++i) {
        value[i*4]   = static_cast<char>(filters.at(i).t);
        value[i*4+1] = filters.at(i).f*config.freqStep+config.freqMin;
        value[i*4+2] = static_cast<int8_t>(filters.at(i).g*2.0);
        value[i*4+3] = filters.at(i).q*config.qStep+config.qMin;
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
    const auto& config = m_model->m_configuration;

    switch (task) {
    case common::FilterTask::Peq: {
        if (value.size()%4 != 0) {
            return;
        }
        std::vector<Model::Filter> filters;
        filters.reserve(value.size()/4);
        for (int i = 0; i < value.size(); i += 4) {
            filters.push_back(Model::Filter(static_cast<common::FilterType>(value.at(i)),
                                            static_cast<uint8_t>((value.at(i+1)-config.freqMin)/config.freqStep),
                                            value.at(i+2)*0.5,
                                            static_cast<uint8_t>((value.at(i+3)-config.qMin)/config.qStep)));
        }
        emit initPeq(filters);

        break;
    }
    case common::FilterTask::Crossover:
    case common::FilterTask::Loudness:
    case common::FilterTask::Invalid:
        break;
    }
}
