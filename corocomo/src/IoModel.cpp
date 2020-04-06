#include "IoModel.h"

#include <common/ble/Types.h>

IoModel* IoModel::s_instance = nullptr;

IoModel* IoModel::instance()
{
    if (s_instance) {
        return s_instance;
    }

    s_instance = new IoModel();
    return s_instance;
}

IoModel::IoModel(QObject *parent) :
    QObject(parent)
{
    onIoCapsReceived({}, {});
}

void IoModel::setService(QZeroProps::QZeroPropsService* service)
{
    m_zpService = service;

    if (m_zpService) {
        // @TODO(mawe): fix this
        connect(m_zpService, &QZeroProps::QZeroPropsService::propertyChanged, this, &IoModel::onPropertyChangedRemotely);
    }
}

QStringList IoModel::inputNames() const
{
    QStringList names;

    for (const auto& i : m_inputs) {
        names << toString(i);
    }

    return names;
}

QStringList IoModel::outputNames() const
{
    QStringList names;

    for (const auto& i : m_outputs) {
        names << toString(i);
    }

    return names;
}

int IoModel::activeInput() const
{
    return m_activeInput;
}

void IoModel::setActiveInput(int i)
{
    m_activeInput = i;
    sendIoConf();
    emit activeInputChanged();
}

int IoModel::activeOutput() const
{
    return m_activeOutput;
}

void IoModel::setActiveOutput(int i)
{
    m_activeOutput = i;
    sendIoConf();
    emit activeOutputChanged();
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

bool IoModel::multiChannelAvailable() const
{
    return m_outputs.at(m_activeOutput).type == common::Spdif ||
           m_outputs.at(m_activeOutput).type == common::Hdmi;
}

void IoModel::startDemo()
{
    onIoCapsReceived({ { common::IoInterfaceType::Bluetooth, false, 1 },
                       { common::IoInterfaceType::Airplay, false, 1 } },
                     { { common::IoInterfaceType::Hdmi, true, 2 },
                       { common::IoInterfaceType::Spdif, true, 3 } });
}

QString IoModel::toString(common::IoInterface interface)
{
    QString string;

    switch (interface.type) {
    case common::IoInterfaceType::Invalid:
        string = "<unknown>";
        break;
    case common::IoInterfaceType::Default:
        string = "Default";
        break;
    case common::IoInterfaceType::Analog:
        string = "Analog";
        break;
    case common::IoInterfaceType::Spdif:
        string = "SPDIF";
        break;
    case common::IoInterfaceType::Hdmi:
        string = "HDMI";
        break;
    case common::IoInterfaceType::Bluetooth:
        string = "Bluetooth";
        break;
    case common::IoInterfaceType::Airplay:
        string = "Airplay";
        break;
    case common::IoInterfaceType::Scream:
        string = "Scream";
        break;
    case common::IoInterfaceType::Max:
        return QString();
    }
    if (interface.number > 0) {
        string += (" " + QString::number(interface.number));
    }

    return string;
}

void IoModel::onPropertyChangedRemotely(const QVariant& uuid, const QByteArray& value)
{
    if (uuid.toUuid().toByteArray(QUuid::WithoutBraces).toStdString() == common::ble::ioCapsCharacteristicUuid) {
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

        onIoCapsReceived(inputs, outputs);
    } else if (uuid.toUuid().toByteArray(QUuid::WithoutBraces).toStdString() == common::ble::ioConfCharacteristicUuid) {
        common::IoInterface i;
        common::IoInterface o;
        for (const auto c : value) {
            common::IoInterface interface = *reinterpret_cast<const common::IoInterface*>(&c);
            if (!interface.isOutput) i = interface;
            else o = interface;
        }

        onIoConfReceived(i, o);
    }
}

void IoModel::onIoCapsReceived(const std::vector<common::IoInterface>& inputs, const std::vector<common::IoInterface>& outputs)
{
    m_inputs.clear();
    m_outputs.clear();

    for (const auto& in : inputs) {
        if (in.number > 1) {
            for (uint8_t i = 0; i < in.number; ++i) {
                m_inputs.push_back( { in.type, false, uint8_t(i+1) } );
            }
        } else {
            m_inputs.push_back( { in.type, false, 0 } );
        }
    }

    for (const auto& out : outputs) {
        if (out.number > 1) {
            for (uint8_t i = 0; i < out.number; ++i) {
                m_outputs.push_back( { out.type, true, uint8_t(i+1) } );
            }
        } else {
            m_outputs.push_back( { out.type, true, 0 } );
        }
    }

    if (m_inputs.empty()) {
        m_inputs = { { common::IoInterfaceType::Invalid, false, 0 } };
    }
    if (m_outputs.empty()) {
        m_outputs = { { common::IoInterfaceType::Invalid, true, 0 } };
    }

    emit iosChanged();
}

void IoModel::onIoConfReceived(const common::IoInterface& input, const common::IoInterface& output)
{
    for (uint i = 0; i < m_inputs.size(); ++i) {
        if (m_inputs.at(i) == input) {
            setActiveInput(i);
        }
    }

    for (uint i = 0; i < m_outputs.size(); ++i) {
        if (m_outputs.at(i) == output) {
            setActiveOutput(i);
        }
    }
}

void IoModel::sendIoConf()
{
    QByteArray value(2, 0);
    auto i = input();
    if (i.number > 0) i.number -= 1; // Correct number to index
    auto o = output();
    if (o.number > 0) o.number -= 1; // Correct number to index
    value[0] = *reinterpret_cast<char*>(&i);
    value[1] = *reinterpret_cast<char*>(&o);

    if (!m_zpService) {
        qWarning() << __func__ << "> No remote service set";
        return;
    }
    m_zpService->setProperty(QUuid(common::ble::ioConfCharacteristicUuid.c_str()), value);
}
