#pragma once

#include <QObject>

#include "Plot.h"

class BodePlotModel : public QObject
{
    Q_OBJECT

public:
    static BodePlotModel* instance();

    Q_INVOKABLE void setFilter(int i, uchar t, uchar f, double g, double q);
    //Q_INVOKABLE void setCrossover(uchar t, uchar f, double g, double q);
    //Q_INVOKABLE void setSubwoofer(uchar t, uchar f, double g, double q);

    const QList<Plot>& plots() const;

private:
    BodePlotModel(QObject* parent = nullptr);

    static BodePlotModel* s_instance;

    std::vector<double> m_freqTable;

    QList<Plot>  m_graphs;

    uchar m_f = 0;
};
