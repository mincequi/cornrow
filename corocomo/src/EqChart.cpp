#include "EqChart.h"

#include <complex>

#include <QPainter>

#include <common/Util.h>

EqChart::EqChart(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
}

void EqChart::setPlotCount(int count)
{
    for (int i = 0; i < count; ++i) {
        QPolygonF poly(m_frequencyTable.size());
        for (uint j = 0; j < m_frequencyTable.size(); ++j) {
            poly[j].rx() = j;
        }
        m_plots.append(poly);
    }
}

void EqChart::setFilter(int i, uchar t, double f, double g, double q)
{
    if (i >= m_plots.size()) return;

    computeResponse({static_cast<common::FilterType>(t), f, g, q}, &m_plots[i]);
    update();
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

QColor EqChart::warningColor() const
{
    return m_warningColor;
}

void EqChart::setWarningColor(const QColor &color)
{
    m_warningColor = color;
}

QColor EqChart::criticalColor() const
{
    return m_criticalColor;
}

void EqChart::setCriticalColor(const QColor &color)
{
    m_criticalColor = color;
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
    QTransform trans;
    trans.scale(width()/(m_frequencyTable.size()-1.0), height()/-33);
    trans.translate(0.0, -7.5);
    painter->setRenderHints(QPainter::Antialiasing, true);

    // Paint plots, except current one
    /*
    painter->setPen(QPen(m_plotColor, 1.0));
    for (auto i = 0; i < m_plots.size(); ++i) {
        if (i == m_currentPlot) continue;
        painter->drawPolyline(trans.map(m_plots.at(i)));
    }
    */

    // Paint sum plot
    painter->setPen(QPen(QColor(0, 0, 0, 0), 1.0));
    QPolygonF sumPlot1(m_frequencyTable.size());
    for (size_t i = 0; i < m_frequencyTable.size(); ++i) {
        sumPlot1[i].rx() = i;
    }
    for (auto& plot : m_plots) {
        for (int i = 0; i < plot.size(); ++i) {
            sumPlot1[i].ry() += plot[i].ry();
        }
    }
    sumPlot1 << QPointF(sumPlot1.back().rx()+1.0, sumPlot1.back().ry());
    sumPlot1 << QPointF(sumPlot1.back().rx(), -30.0);
    sumPlot1 << QPointF(-1.0, -30.0);
    sumPlot1 << QPointF(-1.0, sumPlot1.front().ry());
    sumPlot1 << sumPlot1.front();

    QLinearGradient grad(trans.map(QPointF(0.0, 6.0)), trans.map(QPointF(0.0, -24.0)));
    auto col = m_sumPlotColor;
    col.setAlpha(0);
    grad.setColorAt(1.0, col);
    col.setAlpha(24);
    grad.setColorAt(0.99, col);
    col.setAlpha(32);
    grad.setColorAt(0.9, col);

    col.setAlpha(56);
    grad.setColorAt(0.89, col);
    col.setAlpha(64);
    grad.setColorAt(0.8, col);

    col.setAlpha(88);
    grad.setColorAt(0.79, col);
    col.setAlpha(96);
    grad.setColorAt(0.70, col);

    col.setAlpha(120);
    grad.setColorAt(0.69, col);
    col.setAlpha(128);
    grad.setColorAt(0.60, col);

    col.setAlpha(152);
    grad.setColorAt(0.59, col);
    col.setAlpha(160);
    grad.setColorAt(0.50, col);

    col.setAlpha(184);
    grad.setColorAt(0.49, col);
    col.setAlpha(192);
    grad.setColorAt(0.40, col);

    col.setAlpha(216);
    grad.setColorAt(0.39, col);
    col.setAlpha(224);
    grad.setColorAt(0.30, col);

    col.setAlpha(248);
    grad.setColorAt(0.29, col);
    col.setAlpha(255);
    grad.setColorAt(0.20, col);

    col.setAlpha(255);
    grad.setColorAt(0.19, col);

    grad.setColorAt(0.15, QColor(0x67, 0x3A, 0xB7)); // DeepPurple
    grad.setColorAt(0.10, QColor(0x9C, 0x27, 0xB0)); // Purple
    grad.setColorAt(0.05, m_criticalColor);

    painter->setBrush(QBrush(grad));
    painter->drawPolygon(trans.map(sumPlot1));

    // Paint current plot
    painter->setPen(QPen(m_plotColor, 1.0));
    if (m_currentPlot >= 0 && m_currentPlot < m_plots.size()) {
        painter->drawPolyline(trans.map(m_plots.at(m_currentPlot)));
    }
}

void EqChart::computeResponse(const common::Filter& f, QPolygonF* mags)
{
    common::BiQuad biquad;
    if (!computeBiQuad(44100, f, &biquad)) {
        for (QPointF& m : *mags) {
            m.ry() = 0.0;
        }
        return;
    }

    for (QPointF& m : *mags) {
        double w = 2.0*M_PI*m_frequencyTable.at(m.rx())/44100;
        std::complex<double> z(cos(w), sin(w));
        std::complex<double> numerator = biquad.b0 + (biquad.b1 + biquad.b2*z)*z;
        std::complex<double> denominator = 1.0 + (biquad.a1 + biquad.a2*z)*z;
        std::complex<double> response = numerator / denominator;
        m.ry() = 20.0*log10(abs(response));
    }
}
