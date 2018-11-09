#pragma once

#include <QObject>
#include <QTimer>

#include <ble/Central.h>
#include <common/Types.h>

#include "Model.h"

class BleCentralAdapter : public QObject
{
    Q_OBJECT

public:
    explicit BleCentralAdapter(ble::Central* central,
                               Model* model);
    ~BleCentralAdapter();

    void setPeqDirty();

signals:
    void status(Model::Status status, const QString& errorString = QString());
    void initPeq(const std::vector<Model::Filter>& filters);
    void initXo(const QByteArray& value);
    void initLoudness(const QByteArray& value);

private:
    void doWriteCharc();
    void onStatus(ble::Central::Status status, const QString& errorString);
    void onCharacteristicRead(common::FilterTask task, const QByteArray &value);

    ble::Central*   m_central;
    Model*          m_model;

    QTimer          m_timer;
};


