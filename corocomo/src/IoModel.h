#pragma once

#include <QObject>
#include <QStringList>

#include <common/Types.h>

class BleCentralAdapter;

class IoModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList inputNames READ inputNames NOTIFY iosChanged)
    Q_PROPERTY(QStringList outputNames READ outputNames NOTIFY iosChanged)
    Q_PROPERTY(int activeInput READ activeInput WRITE setActiveInput NOTIFY activeInputChanged)
    Q_PROPERTY(int activeOutput READ activeOutput WRITE setActiveOutput NOTIFY activeOutputChanged)

public:
    static IoModel* init(BleCentralAdapter* adapter);
    static IoModel* instance();

    QStringList inputNames() const;
    QStringList outputNames() const;

    int     activeInput() const;
    void    setActiveInput(int i);

    int     activeOutput() const;
    void    setActiveOutput(int i);

    common::IoInterface input();
    common::IoInterface output();

signals:
    void iosChanged();

    void activeInputChanged();
    void activeOutputChanged();

private:
    IoModel(BleCentralAdapter* adapter, QObject* parent = nullptr);
    void onIoCapsReceived(const std::vector<common::IoInterface>& inputs, const std::vector<common::IoInterface>& ouputs);
    void onIoConfReceived(const common::IoInterface& input, const common::IoInterface& ouput);

    static IoModel* s_instance;

    BleCentralAdapter* m_adapter = nullptr;

    std::vector<common::IoInterface> m_inputs;
    std::vector<common::IoInterface> m_outputs;

    int m_activeInput = 0;
    int m_activeOutput = 0;
};
