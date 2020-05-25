#pragma once

#include <QPolygonF>
#include <QQuickPaintedItem>

#include <common/Types.h>

class BodePlotModel;

class EqChart : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QColor plotColor READ plotColor WRITE setPlotColor)
    Q_PROPERTY(QColor currentPlotColor READ currentPlotColor WRITE setCurrentPlotColor)
    Q_PROPERTY(QColor sumPlotColor READ sumPlotColor WRITE setSumPlotColor)
    Q_PROPERTY(QColor warningColor READ warningColor WRITE setWarningColor)
    Q_PROPERTY(QColor criticalColor READ criticalColor WRITE setCriticalColor)

    Q_PROPERTY(int currentFilter READ currentFilter WRITE setCurrentFilter)

    Q_PROPERTY(BodePlotModel* bodePlot MEMBER m_bodePlot)

public:
    EqChart(QQuickItem *parent = nullptr);

    QColor plotColor() const;
    void setPlotColor(const QColor &color);
    QColor currentPlotColor() const;
    void setCurrentPlotColor(const QColor &color);
    QColor sumPlotColor() const;
    void setSumPlotColor(const QColor &color);
    QColor warningColor() const;
    void setWarningColor(const QColor &color);
    QColor criticalColor() const;
    void setCriticalColor(const QColor &color);

    int currentFilter() const;
    void setCurrentFilter(int i);

    void paint(QPainter *painter) override;

protected:
    std::vector<double>  m_frequencyTable;
    QColor m_plotColor;         // background plots
    QColor m_currentPlotColor;  // current plot
    QColor m_sumPlotColor;      // sum plot
    QColor m_warningColor;
    QColor m_criticalColor;

    int    m_currentFilter = -1;

    BodePlotModel* m_bodePlot = nullptr;
};
// QVector<qreal> is Qt 5.9 compatible. Qt 5.11 also accepts std::vector<double>.
Q_DECLARE_METATYPE(std::vector<double>)
