#ifndef COROCO_TYPES_H
#define COROCO_TYPES_H

#include "common/Types.h"

class QGraphicsItemGroup;
class KPlotObject;

struct FilterPlot {
    FilterPlot();
    ~FilterPlot();

    FilterType    t = FilterType::Peak;
    int     f = 68;
    float   g = 0.0;
    int     q = 17;
    std::vector<float> mags;
    std::vector<float> phases;

    KPlotObject* plot;
};

#endif // COROCO_TYPES_H
