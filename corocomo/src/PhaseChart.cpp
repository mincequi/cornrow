#include "PhaseChart.h"

#include <complex>

#include <QPainter>

#include <common/Util.h>

#include "BodePlotModel.h"

void PhaseChart::paint(QPainter *painter)
{
    QTransform trans;
    trans.translate(0.0, height()/2.0);
    trans.scale(width()/(m_frequencyTable.size()-1.0), 0.9*0.25*height()/M_PI);
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
    painter->setPen(QPen(m_sumPlotColor, 2.0));
    QPolygonF sumPlot1(m_frequencyTable.size());
    for (size_t i = 0; i < m_frequencyTable.size(); ++i) {
        sumPlot1[i].rx() = i;
    }
    for (auto& graph : m_bodePlot->plots()) {
        for (int i = 0; i < graph.phaseSum().size(); ++i) {
            sumPlot1[i].ry() += graph.phaseSum().at(i).y();
        }
    }
    painter->drawPolyline(trans.map(sumPlot1));

    // Paint current plot
    painter->setPen(QPen(m_plotColor, 1.0));
    if (m_currentFilter >= 0 && m_currentFilter < m_bodePlot->plots().size()) {
        for (const auto& graph : m_bodePlot->plots().at(m_currentFilter).phases()) {
            painter->drawPolyline(trans.map(graph));
        }
    }
}
