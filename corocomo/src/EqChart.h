#ifndef EQCHART_H
#define EQCHART_H

#include <QPolygonF>
#include <QQuickPaintedItem>

#include <common/Types.h>

class EqChart : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(std::vector<double> frequencyTable MEMBER m_frequencyTable)
    Q_PROPERTY(int plotCount WRITE setPlotCount)
    Q_PROPERTY(QColor plotColor READ plotColor WRITE setPlotColor)
    Q_PROPERTY(QColor currentPlotColor READ currentPlotColor WRITE setCurrentPlotColor)
    Q_PROPERTY(QColor sumPlotColor READ sumPlotColor WRITE setSumPlotColor)
    Q_PROPERTY(QColor warningColor READ warningColor WRITE setWarningColor)
    Q_PROPERTY(QColor criticalColor READ criticalColor WRITE setCriticalColor)

    Q_PROPERTY(int currentPlot READ currentPlot WRITE setCurrentPlot)

public:
    EqChart(QQuickItem *parent = nullptr);

    Q_INVOKABLE void setFilter(int i, uchar t, double f, double g, double q);
    Q_INVOKABLE void setPlotCount(int i);

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

    int currentPlot() const;
    void setCurrentPlot(int i);

    void paint(QPainter *painter) override;

private:
    void computeResponse(const common::Filter& f, QPolygonF* mags);

    std::vector<double>  m_frequencyTable;
    QColor m_plotColor;         // background plots
    QColor m_currentPlotColor;  // current plot
    QColor m_sumPlotColor;      // sum plot
    QColor m_warningColor;
    QColor m_criticalColor;

    int    m_currentPlot = -1;

    QList<QPolygonF>    m_plots;
};
// QVector<qreal> is Qt 5.9 compatible. Qt 5.11 also accepts std::vector<double>.
Q_DECLARE_METATYPE(std::vector<double>)

#endif // EQCHART_H
