#ifndef GSTDSPCROSSOVER_H
#define GSTDSPCROSSOVER_H

#include <gstreamermm.h>
#include <gstreamermm/private/audiofilter_p.h>

class GstDspCrossover : public Gst::AudioFilter
{
public:
    explicit GstDspCrossover(GstAudioFilter *obj);

    static void class_init(Gst::ElementClass<GstDspCrossover> *klass);

private:
    virtual bool setup_vfunc(const Gst::AudioInfo& info) override;
    virtual Gst::FlowReturn transform_vfunc(const Glib::RefPtr<Gst::Buffer>& inbuf, const Glib::RefPtr<Gst::Buffer>& outbuf) override;
    virtual Gst::FlowReturn transform_ip_vfunc(const Glib::RefPtr<Gst::Buffer>& buf) override;
};

#endif // GSTDSPCROSSOVER_H
