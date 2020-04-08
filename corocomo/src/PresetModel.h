#pragma once

#include <QObject>
#include <QStringList>

#include <common/Types.h>

namespace QtZeroProps {
class QZeroPropsService;
}

class PresetModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList presetNames READ presetNames NOTIFY presetsChanged)
    Q_PROPERTY(int activePreset READ activePreset WRITE setActivePreset NOTIFY activePresetChanged)

public:
    static PresetModel* instance();

    Q_INVOKABLE void setService(QtZeroProps::QZeroPropsService* service);

    QStringList presetNames() const;

    int     activePreset() const;
    Q_INVOKABLE void setActivePreset(int i);

    Q_INVOKABLE void startDemo();

signals:
    void presetsChanged();
    void activePresetChanged();

private:
    PresetModel(QObject* parent = nullptr);

    static QString toString(common::IoInterface interface);

    void onPresetReceived(uint8_t index, uint8_t total, uint8_t active, const QString& name);

    static PresetModel* s_instance;

    QtZeroProps::QZeroPropsService* m_zpService = nullptr;

    QStringList m_presetNames;
    int m_activePreset = 0;
};
