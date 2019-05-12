#pragma once

#include <QPolygonF>

#include <common/Types.h>

class EqGraph
{
public:
    explicit EqGraph(const std::vector<double>& frequencyTable);

    EqGraph& operator=(const EqGraph&);

    void setFilter(const common::Filter& filter);

    const QList<QPolygonF>& graphs() const;
    const QPolygonF& sum() const;

private:
    void resizeGraphs(int newSize);

    void computeResponse(const common::Filter& f, QPolygonF& mags) const;
    static void sumResponses(const QList<QPolygonF>& levels, QPolygonF& sum);

    const std::vector<double>& m_frequencyTable;
    QList<QPolygonF> m_graphs;
    QPolygonF m_sum;
};
