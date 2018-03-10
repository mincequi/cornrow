#include "GstDspPeq.h"

#include <iostream>

GstDspPeq::GstDspPeq(GstAudioFilter *obj)
    : Glib::ObjectBase(typeid(GstDspPeq)),
      Gst::AudioFilter(obj),
      m_filterCount(*this, "filter-count", 1)
{
    set_in_place(false);
    set_passthrough(false);
    filterCount().signal_changed().connect(sigc::mem_fun(*this, &GstDspPeq::onFilterCountChanged));
    onFilterCountChanged();
}

GType GstDspPeq::get_base_type()
{
    return Gst::AudioFilter::get_base_type();
}

void GstDspPeq::class_init(Gst::ElementClass<GstDspPeq> *klass)
{
    klass->set_metadata("Parametric Equalizer",
                        "Filter/Effect/Audio",
                        "A fully parametric equalizer",
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

Glib::PropertyProxy<unsigned> GstDspPeq::filterCount()
{
    return m_filterCount.get_proxy();
}

std::vector<Glib::RefPtr<GstDspBiquad>>& GstDspPeq::biquads()
{
    return m_biquads;
}

bool GstDspPeq::setup_vfunc(const Gst::AudioInfo& info)
{
    if (m_audioInfo.is_equal(info)) return true;

    m_mutex.lock();
    m_audioInfo = info;
    for (auto& biquad : m_biquads) {
        biquad->setAudioInfo(info);
    }
    m_mutex.unlock();

    std::cerr << "bpf: " << info.get_bpf() << "rate: " << info.get_rate() << "channels: " << info.get_channels() << std::endl;

    return true;
}

Gst::FlowReturn GstDspPeq::transform_vfunc(const Glib::RefPtr<Gst::Buffer>& inbuf, const Glib::RefPtr<Gst::Buffer>& outbuf)
{
    Gst::MapInfo inMap;
    Gst::MapInfo outMap;

    inbuf->map(inMap, Gst::MAP_READ);
    outbuf->map(outMap, Gst::MAP_WRITE);

    float* inData = (float*)(inMap.get_data());
    float* outData = (float*)(outMap.get_data());
    uint   sampleCount = inMap.get_size()/sizeof(float);

    for (uint i = 0; i < sampleCount; ++i) {
        outData[i] = inData[i];
    }

    m_mutex.lock();
    for (auto& biquad : m_biquads) {
        biquad->update();
        biquad->process(outData, sampleCount);
    }
    m_mutex.unlock();

    inbuf->unmap(inMap);
    outbuf->unmap(outMap);

    return Gst::FlowReturn::FLOW_OK;
}

Gst::FlowReturn GstDspPeq::transform_ip_vfunc(const Glib::RefPtr<Gst::Buffer>& /*buf*/)
{
    std::cerr << "transform_ip" << std::endl;

    return Gst::FlowReturn::FLOW_OK;
}

void GstDspPeq::onFilterCountChanged()
{
    std::cerr << "onFilterCountChanged: " << m_filterCount << std::endl;

    if (m_biquads.size() == static_cast<size_t>(m_filterCount)) return;

    m_mutex.lock();
    if (m_biquads.size() < m_filterCount) {
        for (unsigned i = m_biquads.size(); i < m_filterCount; ++i) {
            Glib::RefPtr<GstDspBiquad> instance = Glib::RefPtr<GstDspBiquad>::cast_dynamic(Gst::ElementFactory::create_element("biquad", Glib::ustring::compose("filter%1.", i)));
            instance->setAudioInfo(m_audioInfo);
            m_biquads.push_back(instance);
        }
    } else {
        for (uint i = m_filterCount; i < m_biquads.size(); ++i) {
            m_biquads[i]->gain().set_value(0.0);
        }
    }
    m_mutex.unlock();
}
