#pragma once

#include <QList>
#include <QPolygonF>

#include <common/Types.h>

class Config;

class Plot
{
public:
    explicit Plot(const Config& config);

    Plot& operator=(const Plot&);

    // @TODO(mawe): maybe set filter (frequency) index based
    void setFilter(const common::Filter& filter);
    void setFrequencyIndex(uchar i);

    const QList<QPolygonF>& mags() const;
    const QPolygonF& magSum() const;

    const QList<QPolygonF>& phases() const;
    const QPolygonF& phaseSum() const;

private:
    void resizePolys(int newSize, QList<QPolygonF>& polys);

    void computeResponse(const common::Filter& f, QPolygonF& mags, QPolygonF& phases) const;
    static void sumMags(const QList<QPolygonF>& mags, QPolygonF& magSum);
    void sumPhases(const QList<QPolygonF>& phases, QPolygonF& phaseSum);

    const Config& m_config;

    uchar m_f = 0;

    QList<QPolygonF> m_mags;
    QPolygonF m_magSum;

    QList<QPolygonF> m_phases;
    QPolygonF m_phaseSum;
};
