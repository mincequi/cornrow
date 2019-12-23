#pragma once

#include "EqChart.h"

class SoftClipChart : public EqChart
{
    Q_OBJECT

public:
    SoftClipChart();

    void paint(QPainter *painter) override;

    Q_INVOKABLE void setInputRange(double);
    Q_INVOKABLE void setClipping(double);

private:
    double  m_inputRange = 1.0;
    double  m_clipping = 0.0;
    std::vector<double>  m_sineTable;
};
