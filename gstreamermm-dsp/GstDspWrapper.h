#ifndef GSTDSPWRAPPER_H
#define GSTDSPWRAPPER_H

#include <glibmm/refptr.h>

#include "common/IControllable.h"

class GstDspPeq;
namespace Gst {
class Pipeline;
}

class GstDspWrapper : public IControllable
{
public:
    GstDspWrapper();
    ~GstDspWrapper();

private:
    virtual void setPassthrough(bool passthrough)       override;

    virtual void setFilterCount(uint8_t i)              override;
    virtual void setFilterType(uint8_t i, Type type)    override;
    virtual void setFilterFreq(uint8_t i, float f)      override;
    virtual void setFilterGain(uint8_t i, float g)      override;
    virtual void setFilterQ(uint8_t i,    float q)      override;

    void increaseFilterCount(uint8_t i);

    Glib::RefPtr<Gst::Pipeline> m_pipeline;
    Glib::RefPtr<GstDspPeq>     m_peq;
};

#endif // GSTDSPWRAPPER_H
