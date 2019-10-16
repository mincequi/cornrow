#pragma once

#include <QObject>

#include "Plot.h"

class Config;

class BodePlotModel : public QObject
{
    Q_OBJECT

public:
    static BodePlotModel* init(const Config& configuration);
    static BodePlotModel* instance();

    Q_INVOKABLE void setFilter(int i, uchar t, uchar f, double g, double q);

    const QList<Plot>& plots() const;

private:
    BodePlotModel(const Config& config, QObject* parent = nullptr);

    static BodePlotModel* s_instance;

    const Config&   m_config;

    QList<Plot>  m_graphs;

    uchar m_f = 0;
};
