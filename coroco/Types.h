#ifndef COROCO_TYPES_H
#define COROCO_TYPES_H

#include "common/Types.h"

class QGraphicsItemGroup;

struct Filter {
    ~Filter();

    Type    t = Type::Peak;
    int     f = 68;
    float   g = 0.0;
    int     q = 17;
    std::vector<float> mags;
    std::vector<float> phases;
};

#endif // COROCO_TYPES_H
