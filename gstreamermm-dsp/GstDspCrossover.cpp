#include "GstDspCrossover.h"

GstDspCrossover::GstDspCrossover(GstAudioFilter *obj)
    : Glib::ObjectBase(typeid(GstDspCrossover)),
      Gst::AudioFilter(obj)
{
}

void GstDspCrossover::class_init(Gst::ElementClass<GstDspCrossover> *klass)
{
    klass->set_metadata("Audio Crossover",
                        "Filter/Effect/Audio",
                        "Realizes an audio crossover",
                        "Manuel Weichselbaumer <mincequi@web.de>");

    Glib::ustring capsString = Glib::ustring::compose(
                "audio/x-raw, "
                "format=(string)%1, "
                "rate=(int){44100,48000}, "
                "channels=(int)2, "
                "layout=(string)interleaved", GST_AUDIO_NE(F32));
    auto caps = Gst::Caps::create_from_string(capsString);

    auto sink = Gst::PadTemplate::create("sink", Gst::PAD_SINK, Gst::PAD_ALWAYS, caps);
    auto src  = Gst::PadTemplate::create("src", Gst::PAD_SRC, Gst::PAD_ALWAYS, caps);
    klass->add_pad_template(sink);
    klass->add_pad_template(src);
}

bool GstDspCrossover::setup_vfunc(const Gst::AudioInfo& info)
{
    return true;
}

Gst::FlowReturn GstDspCrossover::transform_vfunc(const Glib::RefPtr<Gst::Buffer>& inbuf, const Glib::RefPtr<Gst::Buffer>& outbuf)
{
    return Gst::FlowReturn::FLOW_OK;
}

Gst::FlowReturn GstDspCrossover::transform_ip_vfunc(const Glib::RefPtr<Gst::Buffer>& inbuf)
{
    return Gst::FlowReturn::FLOW_OK;
}
