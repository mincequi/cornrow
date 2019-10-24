#pragma once

#include <QObject>
#include <QTimer>

#include <ble/Client.h>
#include <common/Types.h>

#include "Model.h"

class BleCentralAdapter : public QObject
{
    Q_OBJECT

public:
    explicit BleCentralAdapter(ble::Client* central,
                               Model* model,
                               IoModel* ioModel);
    ~BleCentralAdapter();

    void setDirty(common::ble::CharacteristicType group);
    void setDirty(const std::string& uuid);

signals:
    void status(Model::Status status, const QString& errorString = QString());
    void filtersReceived(common::ble::CharacteristicType task, const std::vector<Model::Filter>& filters);
    void ioCapsReceived(const std::vector<common::IoInterface>& inputs, const std::vector<common::IoInterface>& ouputs);
    void ioConfReceived(common::IoInterface input, common::IoInterface ouput);

private:
    void doWriteCharc();
    void onStatus(ble::Client::Status status, const QString& errorString);
    void onCharacteristicRead(const std::string& uuid, const QByteArray& value);

    ble::Client*    m_central;
    Model*          m_model;
    IoModel*        m_ioModel;

    QTimer          m_timer;

    Q_DECLARE_FLAGS(FilterGroups, common::ble::CharacteristicType)
    FilterGroups m_dirtyFilterGroups;
    std::set<std::string> m_dirtyCharcs;
};


