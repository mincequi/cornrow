#include "GstDspLoudness.h"

#include <iostream>

GstDspLoudness::GstDspLoudness(GstAudioFilter *obj)
    : Glib::ObjectBase(typeid(GstDspLoudness)),
      Gst::AudioFilter(obj)
{
}

void GstDspLoudness::class_init(Gst::ElementClass<GstDspLoudness> *klass)
{
    klass->set_metadata("Loudness Filter",
                        "Filter/Effect/Audio",
                        "Applies an equal-loudness contour to perceive constant loudness for different volume levels",
                        "Manuel Weichselbaumer <mincequi@web.de>");

    Glib::ustring capsString = Glib::ustring::compose(
                "audio/x-raw, "
                "format=(string)S16LE, "
                "rate=(int)44100, "
                "channels=(int)2, "
                "layout=(string)interleaved", GST_AUDIO_NE(S16));
    auto caps = Gst::Caps::create_from_string(capsString);

    auto sink = Gst::PadTemplate::create("sink", Gst::PAD_SINK, Gst::PAD_ALWAYS, Gst::Caps::create_any());
    auto src  = Gst::PadTemplate::create("src", Gst::PAD_SRC, Gst::PAD_ALWAYS, Gst::Caps::create_any());
    klass->add_pad_template(sink);
    klass->add_pad_template(src);
}

Gst::FlowReturn GstDspLoudness::transform_ip_vfunc(const Glib::RefPtr<Gst::Buffer>& /*buf*/)
{
    std::cerr << "transform_ip_vfunc" << std::endl;

    return Gst::FLOW_OK;
}
