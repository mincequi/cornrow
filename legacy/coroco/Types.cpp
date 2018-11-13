#include "Types.h"

#include <QPen>

#include <kplotobject.h>

FilterPlot::FilterPlot()
{
    plot = new KPlotObject(Qt::darkBlue, KPlotObject::Lines);
    plot->setLinePen(QPen(Qt::darkBlue, 1.5));
}

FilterPlot::~FilterPlot()
{
}
