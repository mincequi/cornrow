#include "EqChart.h"

#include <complex>

#include <QPainter>

#include "common/Util.h"

EqChart::EqChart(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    setFilterCount(1);
    setFilter(0, {FilterType::Peak, 100.0, -27.0, 0.707});
}

void EqChart::setFilterCount(int i)
{
    if (i == m_plots.size()) return;

    int diff = i - m_plots.size();
    if (diff > 0) {
        m_plots.reserve(diff);
        while (diff--) {
            QPolygonF poly(twelfthOctaveBandsTable.size());
            for (int i = 0; i < twelfthOctaveBandsTable.size(); ++i) {
                poly[i].rx() = i;
            }
            m_plots.append(poly);
        }
    } else if (diff < 0) {
        m_plots.erase(m_plots.end() + diff, m_plots.end());
    }
}

void EqChart::setFilter(int i, const Filter& filter)
{
    if (i >= m_plots.size()) return;

    computeResponse(filter, &m_plots[i]);
}

QColor EqChart::plotColor() const
{
    return m_plotColor;
}

void EqChart::setPlotColor(const QColor &color)
{
    m_plotColor = color;
}

QColor EqChart::currentPlotColor() const
{
    return m_currentPlotColor;
}

void EqChart::setCurrentPlotColor(const QColor &color)
{
    m_currentPlotColor = color;
}

QColor EqChart::sumPlotColor() const
{
    return m_sumPlotColor;
}

void EqChart::setSumPlotColor(const QColor &color)
{
    m_sumPlotColor = color;
}

int EqChart::currentPlot() const
{
    return m_currentPlot;
}
void EqChart::setCurrentPlot(int i)
{
    m_currentPlot = i;
}

void EqChart::paint(QPainter *painter)
{
    painter->setPen(QPen(m_currentPlotColor, 2.0));
    QTransform trans;
    trans.scale(width()/(twelfthOctaveBandsTable.size()-1.0), height()/-36.0);
    trans.translate(0.0, -9.0);
    painter->setRenderHints(QPainter::Antialiasing, true);

    for (const auto& p : m_plots) {
        painter->drawPolyline(trans.map(p));
    }
}

void EqChart::computeResponse(const Filter& f, QPolygonF* mags)
{
    BiQuad biquad;
    if (!computeBiQuad(48000, f, &biquad)) return;

    for (QPointF& m : *mags) {
        double w = 2.0*M_PI*twelfthOctaveBandsTable.at(m.rx())/48000;
        std::complex<double> z(cos(w), sin(w));
        std::complex<double> numerator = biquad.b0 + (biquad.b1 + biquad.b2*z)*z;
        std::complex<double> denominator = 1.0 + (biquad.a1 + biquad.a2*z)*z;
        std::complex<double> response = numerator / denominator;
        m.ry() = 20.0*log10(abs(response));
    }

    return;
}
