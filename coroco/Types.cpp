#include "Types.h"

#include <QPen>

#include <KPlotObject>

Filter::Filter()
{
    plot = new KPlotObject(Qt::darkBlue, KPlotObject::Lines);
    plot->setLinePen(QPen(Qt::darkBlue, 1.5));
}

Filter::~Filter()
{
}
