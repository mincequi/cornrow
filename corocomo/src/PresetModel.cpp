#include "PresetModel.h"

#include <QTimer>

PresetModel* PresetModel::s_instance = nullptr;

PresetModel* PresetModel::instance()
{
    if (s_instance) {
        return s_instance;
    }

    s_instance = new PresetModel();
    return s_instance;
}

PresetModel::PresetModel(QObject* parent) :
    QObject(parent)
{
}

void PresetModel::setService(QZeroProps::QZeroPropsService* service)
{
    m_zpService = service;
}

QStringList PresetModel::presetNames() const
{
    return m_presetNames;
}

int PresetModel::activePreset() const
{
    return m_activePreset;
}

void PresetModel::setActivePreset(int i)
{
    m_activePreset = i;
    emit activePresetChanged();
}

void PresetModel::startDemo()
{
    auto timer = new QTimer(this);
    timer->setSingleShot(false);
    timer->callOnTimeout([&]() {
        QStringList list = { "Bla", "Blub", "Very long name", "Even longer name", "a", "b", "Two way", "LFE", "Living room" };
        static uint8_t i = 0;
        if (i >= list.size()) i = 0;
        onPresetReceived(i, i, i, list.at(i));
        ++i;
    });
    timer->start(2000);
}

void PresetModel::onPresetReceived(uint8_t index, uint8_t total, uint8_t active, const QString& name)
{
    // Resize to total
    int diff = total - m_presetNames.size();
    if (diff > 0) {
        m_presetNames.reserve(diff);
        while (diff--) {
            m_presetNames.append("Loading...");
        }
    } else if (diff < 0) {
        m_presetNames.erase(m_presetNames.end() + diff, m_presetNames.end());
    }

    // Set preset name at index
    m_presetNames.insert(index, name);

    // Set active preset
    m_activePreset = active;

    emit presetsChanged();
    emit activePresetChanged();
}
