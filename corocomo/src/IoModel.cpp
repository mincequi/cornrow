#include "IoModel.h"

#include "BleCentralAdapter.h"

IoModel* IoModel::s_instance = nullptr;

IoModel* IoModel::instance()
{
    return s_instance;
}

IoModel* IoModel::init(BleCentralAdapter* adapter)
{
    if (s_instance) {
        return s_instance;
    }

    s_instance = new IoModel(adapter);
    return s_instance;
}

IoModel::IoModel(BleCentralAdapter* adapter, QObject *parent) :
    QObject(parent),
    m_adapter(adapter)
{
    connect(m_adapter, &BleCentralAdapter::ioCapsReceived, this, &IoModel::onIoCapsReceived);
}

QStringList IoModel::inputNames() const
{
    QStringList inputs;

    for (const auto& i : m_inputs) {
        switch (i.type) {
        case common::IoInterfaceType::Bluetooth:
            inputs << "Bluetooth";
            break;
        case common::IoInterfaceType::Airplay:
            inputs << "Airplay";
            break;
        default:
            break;
        }
    }

    return inputs;
}

QStringList IoModel::outputNames() const
{
    QStringList outputs;
    QString output;

    for (const auto& o : m_outputs) {
        switch (o.type) {
        case common::IoInterfaceType::Default:
            output = "Default";
            break;
        case common::IoInterfaceType::Analog:
            output = "Analog";
            break;
        case common::IoInterfaceType::Spdif:
            output = "SPDIF";
            break;
        case common::IoInterfaceType::Hdmi:
            output = "HDMI";
            break;
        default:
            continue;
        }
        if (o.number > 1) {
            for (auto i = 1; i <= o.number; ++i) {
                outputs << (output + " " + QString::number(i));
            }
        } else if (o.number == 1) {
            outputs << output;
        }
    }

    return outputs;
}

int IoModel::activeInput() const
{
    return m_activeInput;
}

void IoModel::setActiveInput(int i)
{
    m_activeInput = i;
    m_adapter->setDirty(common::FilterGroup::IoCaps);
}

int IoModel::activeOutput() const
{
    return m_activeOutput;
}

void IoModel::setActiveOutput(int i)
{
    m_activeOutput = i;
}

common::IoInterface IoModel::input()
{
    if (m_activeInput >= static_cast<int>(m_inputs.size())) {
        return { common::IoInterfaceType::Invalid, false, 0 };
    }

    return m_inputs.at(m_activeInput);
}

common::IoInterface IoModel::output()
{
    if (m_activeOutput >= static_cast<int>(m_outputs.size())) {
        return { common::IoInterfaceType::Invalid, true, 0 };
    }

    return m_outputs.at(m_activeOutput);
}

void IoModel::onIoCapsReceived(const std::vector<common::IoInterface>& inputs, const std::vector<common::IoInterface>& ouputs)
{
    m_inputs = inputs;
    m_outputs = ouputs;

    emit iosChanged();
}
