#ifndef GSTDSPWRAPPER_H
#define GSTDSPWRAPPER_H

#include <glibmm/refptr.h>

#include "common/IControllable.h"

class GstDspPeq;
namespace Gst {
class Caps;
class Element;
class Pipeline;
}

class GstDspWrapper : public IControllable
{
public:
    GstDspWrapper();
    ~GstDspWrapper();

    bool createPipeline(const Config& config);
    void destroyPipeline();

private:
    virtual void setPassthrough(bool passthrough)       override;

    virtual std::vector<::Preset> getPresets()          override;
    virtual ::Preset getPreset(const std::string& name) override;
    virtual std::vector<std::string> getPresetNames()   override;
    virtual void setPreset(const std::string& name)     override;
    virtual void savePreset(const ::Preset& preset)     override;
    virtual void renamePreset(const std::string& oldName, const std::string& newName)   override;
    virtual void deletePreset(const std::string& name)  override;

    virtual void setFilterCount(uint8_t i)              override;
    virtual void setFilterType(uint8_t i, FilterType type)    override;
    virtual void setFilterFreq(uint8_t i, float f)      override;
    virtual void setFilterGain(uint8_t i, float g)      override;
    virtual void setFilterQ(uint8_t i,    float q)      override;

    void increaseFilterCount(uint8_t i);

    Glib::RefPtr<Gst::Pipeline> m_pipeline;
    Glib::RefPtr<Gst::Element>  m_src;
    Glib::RefPtr<Gst::Element>  m_sink;
    Glib::RefPtr<GstDspPeq>     m_peq;
};

#endif // GSTDSPWRAPPER_H
