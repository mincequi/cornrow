#pragma once

#include <QObject>

#include <ble/Central.h>
#include <common/Types.h>

#include "Model.h"

class BleCentralAdapter : public QObject
{
    Q_OBJECT

public:
    explicit BleCentralAdapter(ble::Central* central);
    ~BleCentralAdapter();

    void setFilters(const std::vector<Model::Filter>& filters);

signals:
    void status(Model::Status status, const QString& errorString = QString());
    void peq(const std::vector<Model::Filter>& filters);
    void crossover(const QByteArray& value);
    void loudness(const QByteArray& value);

private:
    void onStatus(ble::Central::Status status, const QString& errorString);
    void onCharacteristicRead(common::FilterTask task, const QByteArray &value);

    ble::Central* m_central;
};


