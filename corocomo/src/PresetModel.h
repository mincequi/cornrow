#pragma once

#include <QObject>
#include <QStringList>

#include <common/Types.h>

class BleCentralAdapter;

class PresetModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList presetNames READ presetNames NOTIFY presetsChanged)
    Q_PROPERTY(int activePreset READ activePreset WRITE setActivePreset NOTIFY activePresetChanged)

public:
    static PresetModel* init(BleCentralAdapter* adapter);
    static PresetModel* instance();

    QStringList presetNames() const;

    int     activePreset() const;
    Q_INVOKABLE void setActivePreset(int i);

    Q_INVOKABLE void startDemo();

signals:
    void presetsChanged();
    void activePresetChanged();

private:
    PresetModel(BleCentralAdapter* adapter, QObject* parent = nullptr);

    static QString toString(common::IoInterface interface);

    void onPresetReceived(uint8_t index, uint8_t total, uint8_t active, const QString& name);

    static PresetModel* s_instance;

    BleCentralAdapter* m_adapter = nullptr;

    QStringList m_presetNames;
    int m_activePreset = 0;
};
