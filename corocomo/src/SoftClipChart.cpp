#include "SoftClipChart.h"

#include <complex>

#include <QPainter>

#include <common/Util.h>

#include "BodePlotModel.h"

SoftClipChart::SoftClipChart() :
    m_sineTable(360)
{
    for (uint i = 0; i < 360; ++i) {
        m_sineTable.at(i) = sin(i*M_PI/-180.0);
    }
}

void SoftClipChart::setClipping(double factor)
{
    m_clipping = factor;
    update();
}

void SoftClipChart::setInputRange(double range)
{
    m_inputRange = range;
    update();
}

void SoftClipChart::paint(QPainter *painter)
{
    QTransform trans;
    trans.translate(0.0, height()/2.0);
    trans.scale(width()/(m_sineTable.size()-1.0), 0.9*0.25*height());
    painter->setRenderHints(QPainter::Antialiasing, true);

    QPolygonF inPlot(m_sineTable.size());
    for (size_t i = 0; i < m_sineTable.size(); ++i) {
        inPlot[i].rx() = i;
        inPlot[i].ry() = m_sineTable.at(i)*m_inputRange;
    }
    QPolygonF sumPlot1(m_sineTable.size());
    for (size_t i = 0; i < m_sineTable.size(); ++i) {
        sumPlot1[i].rx() = i;
        sumPlot1[i].ry() = inPlot[i].ry() - m_inputRange*m_clipping/3.0*pow(inPlot[i].ry(), 3.0);
    }

    // Paint sum plot
    painter->setPen(QPen(m_sumPlotColor, 2.0));
    painter->drawPolyline(trans.map(sumPlot1));

    // Paint current plot
    painter->setPen(QPen(m_plotColor, 1.0));
    painter->drawPolyline(trans.map(inPlot));

    // Prepare font
    auto font = painter->font();
    font.setPointSize(12);
    painter->setFont(font);
    painter->setOpacity(0.5);

    auto pos = trans.map(QPointF(0, 1.0));
    pos.rx() -= 4.0;
    pos.ry() -= 6.0; //font.pointSize()/2.0;
    painter->drawText(QRectF(pos.x(), pos.y(), width(), 50.0), Qt::AlignRight, "-1.0");

    pos = trans.map(QPointF(0, 2.0));
    pos.rx() -= 4.0;
    pos.ry() -= 6.0; //font.pointSize()/2.0;
    painter->drawText(QRectF(pos.x(), pos.y(), width(), 50.0), Qt::AlignRight, "-2.0");

    pos = trans.map(QPointF(0, -1.0));
    pos.rx() -= 4.0;
    pos.ry() -= 6.0; //font.pointSize()/2.0;
    painter->drawText(QRectF(pos.x(), pos.y(), width(), 50.0), Qt::AlignRight, "1.0");

    pos = trans.map(QPointF(0, -2.0));
    pos.rx() -= 4.0;
    pos.ry() -= 6.0; //font.pointSize()/2.0;
    painter->drawText(QRectF(pos.x(), pos.y(), width(), 50.0), Qt::AlignRight, "2.0");
}
