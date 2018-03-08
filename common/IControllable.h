#ifndef ICONTROLLABLE_H
#define ICONTROLLABLE_H

#include "Types.h"

class IControllable
{
public:
    virtual ~IControllable() {}

    virtual void setPassthrough(bool passthrough)       = 0;

    virtual void setFilterCount(uint8_t i)              = 0;
    virtual void setFilterType(uint8_t i, Type type)    = 0;
    virtual void setFilterFreq(uint8_t i, float f)      = 0;
    virtual void setFilterGain(uint8_t i, float g)      = 0;
    virtual void setFilterQ(uint8_t i,    float q)      = 0;
};

#endif // ICONTROLLABLE_H
