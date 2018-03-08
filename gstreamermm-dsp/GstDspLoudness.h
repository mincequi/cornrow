#ifndef GSTDSPLOUDNESS_H
#define GSTDSPLOUDNESS_H

#include <gstreamermm.h>
#include <gstreamermm/private/audiofilter_p.h>

class GstDspLoudness : public Gst::AudioFilter
{
public:
    explicit GstDspLoudness(GstAudioFilter *obj);

    static void class_init(Gst::ElementClass<GstDspLoudness> *klass);
    virtual Gst::FlowReturn transform_ip_vfunc(const Glib::RefPtr<Gst::Buffer>& buf) override;

};

#endif // GSTDSPLOUDNESS_H
