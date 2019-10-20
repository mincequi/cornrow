#include "BleCentralAdapter.h"

#include <ble/Client.h>

#include "IoModel.h"
#include "Model.h"

BleCentralAdapter::BleCentralAdapter(ble::Client* central, Model* model, IoModel* ioModel)
    : QObject(central),
      m_central(central),
      m_model(model),
      m_ioModel(ioModel)
{
    connect(m_central, qOverload<common::FilterGroup, const QByteArray&>(&ble::Client::characteristicRead),
            this, qOverload<common::FilterGroup, const QByteArray&>(&BleCentralAdapter::onCharacteristicRead));
    connect(m_central, qOverload<const std::string&, const QByteArray&>(&ble::Client::characteristicRead),
            this, qOverload<const std::string&, const QByteArray&>(&BleCentralAdapter::onCharacteristicRead));
    connect(m_central, &ble::Client::status, this, &BleCentralAdapter::onStatus);

    m_timer.setInterval(200);
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, &BleCentralAdapter::doWriteCharc);
}

BleCentralAdapter::~BleCentralAdapter()
{
}

void BleCentralAdapter::setDirty(common::FilterGroup group)
{
    // @TODO(mawe): remove FilterGroup type
    m_dirtyFilterGroups.setFlag(group);

    if (!m_timer.isActive()) {
        m_timer.start();
    }
}

void BleCentralAdapter::setDirty(const std::string& uuid)
{
    m_dirtyCharcs.insert(uuid);

    if (!m_timer.isActive()) {
        m_timer.start();
    }
}

void BleCentralAdapter::doWriteCharc()
{
    const auto& filters = m_model->m_filters;
    const auto& config = m_model->m_config;

    if (m_dirtyFilterGroups.testFlag(common::FilterGroup::Peq)) {
        QByteArray value(config.peqFilterCount*4, 0);
        for (int i = 0; i < config.peqFilterCount; ++i) {
            value[i*4]   = static_cast<char>(filters.at(i).t);
            value[i*4+1] = filters.at(i).f*config.freqStep+config.freqMin;
            value[i*4+2] = static_cast<int8_t>(filters.at(i).g*2.0);
            value[i*4+3] = filters.at(i).q*config.qStep+config.qMin;
        }
        m_central->writeCharacteristic(common::FilterGroup::Peq, value);
        m_dirtyFilterGroups.setFlag(common::FilterGroup::Peq, false);
    }

    if (m_dirtyFilterGroups.testFlag(common::FilterGroup::Aux)) {
        QByteArray value((filters.count() - config.peqFilterCount)*4, 0);
        for (int i = 0; i < (filters.count()-config.peqFilterCount); ++i) {
            value[i*4]   = static_cast<char>(filters.at(i+config.peqFilterCount).t);
            value[i*4+1] = filters.at(i+config.peqFilterCount).f*config.freqStep+config.freqMin;
            value[i*4+2] = static_cast<int8_t>(filters.at(i+config.peqFilterCount).g*2.0);
            value[i*4+3] = filters.at(i+config.peqFilterCount).q*config.qStep+config.qMin;
        }
        m_central->writeCharacteristic(common::FilterGroup::Aux, value);
        m_dirtyFilterGroups.setFlag(common::FilterGroup::Aux, false);
    }

    if (m_dirtyCharcs.count(common::ble::ioConfCharacteristicUuid)) {
        QByteArray value(2, 0);
        auto i = m_ioModel->input();
        auto o = m_ioModel->output();
        value[0] = *reinterpret_cast<char*>(&i);
        value[1] = *reinterpret_cast<char*>(&o);
        m_central->writeCharacteristic(common::ble::ioConfCharacteristicUuid, value);
    }
}

void BleCentralAdapter::onStatus(ble::Client::Status _status, const QString& statusText)
{
    switch (_status) {
    case ble::Client::Status::NoBluetooth:
        emit status(Model::Status::NoBluetooth);
        return;
    case ble::Client::Status::Discovering:
        emit status(Model::Status::Discovering, statusText);
        return;
    case ble::Client::Status::Connecting:
        emit status(Model::Status::Connecting, statusText);
        return;
    case ble::Client::Status::Connected:
        emit status(Model::Status::Connected);
        return;
    case ble::Client::Status::Timeout:
        emit status(Model::Status::Timeout);
        return;
    case ble::Client::Status::Lost:
        emit status(Model::Status::Lost);
        return;
    case ble::Client::Status::Error:
        emit status(Model::Status::Error, statusText);
        return;
    }
}

void BleCentralAdapter::onCharacteristicRead(common::FilterGroup task, const QByteArray& value)
{
    const auto& config = m_model->m_config;

    switch (task) {
    case common::FilterGroup::Peq:
    case common::FilterGroup::Aux: {
        if (value.size()%4 != 0) {
            return;
        }
        std::vector<Model::Filter> filters;
        filters.reserve(value.size()/4);
        for (int i = 0; i < value.size(); i += 4) {
            filters.push_back(Model::Filter(static_cast<common::FilterType>(value.at(i)),
                                            (static_cast<uint8_t>(value.at(i+1)-config.freqMin)/config.freqStep),
                                            value.at(i+2)*0.5,
                                            (static_cast<uint8_t>(value.at(i+3)-config.qMin)/config.qStep)));
        }
        emit filtersReceived(task, filters);

        break;
    }

    case common::FilterGroup::Invalid:
        break;
    }
}


void BleCentralAdapter::onCharacteristicRead(const std::string& uuid, const QByteArray& value)
{
    if (uuid == common::ble::ioCapsCharacteristicUuid) {
        std::vector<common::IoInterface> inputs;
        std::vector<common::IoInterface> outputs;
        for (const auto& c : value) {
            const common::IoInterface* i = reinterpret_cast<const common::IoInterface*>(&c);
            if (i->isOutput) {
                outputs.push_back(*i);
            } else {
                inputs.push_back(*i);
            }
        }
        emit ioCapsReceived(inputs, outputs);
    }
    else if (uuid == common::ble::ioConfCharacteristicUuid) {
        common::IoInterface i;
        common::IoInterface o;

        for (const auto c : value) {
            common::IoInterface interface = *reinterpret_cast<const common::IoInterface*>(&c);
            if (!interface.isOutput) i = interface;
            else o = interface;
        }

        emit ioConfReceived(i, o);
    }
}
