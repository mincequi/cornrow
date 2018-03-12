#ifndef GSTDSPPEQ_H
#define GSTDSPPEQ_H

#include <gstreamermm.h>
#include <gstreamermm/private/audiofilter_p.h>
#include <gstreamermm/private/childproxy_p.h>

#include "GstDspBiquad.h"

class GstDspPeq : public Gst::AudioFilter, public Gst::ChildProxy
{
public:
    typedef ::Gst::AudioFilter::BaseClassType BaseClassType;
    typedef ::Gst::AudioFilter::BaseObjectType BaseObjectType;
    typedef ::Gst::AudioFilter::CppClassType CppClassType;

    explicit GstDspPeq(GstAudioFilter *obj);

    static GType get_base_type() G_GNUC_CONST;
    static void class_init(Gst::ElementClass<GstDspPeq> *klass);

    Glib::PropertyProxy<unsigned> filterCount();
    std::vector<Glib::RefPtr<GstDspBiquad>>& biquads();

private:
    virtual bool setup_vfunc(const Gst::AudioInfo& info) override;
    static GstFlowReturn transform_ip(GstBaseTransform* self, GstBuffer* buf);
    void process(GstBuffer* buf);

    void onFilterCountChanged();

    Glib::Property<unsigned> m_filterCount;
    Gst::AudioInfo           m_audioInfo;
    std::vector<Glib::RefPtr<GstDspBiquad>> m_biquads;

    std::mutex m_mutex;
};

#endif // GSTDSPPEQ_H
