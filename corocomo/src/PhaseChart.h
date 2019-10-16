#pragma once

#include "EqChart.h"

class PhaseChart : public EqChart
{
    Q_OBJECT

public:
    using EqChart::EqChart;

    void paint(QPainter *painter) override;
};
