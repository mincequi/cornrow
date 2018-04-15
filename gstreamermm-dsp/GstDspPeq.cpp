#include "GstDspPeq.h"

#include <iostream>

GstDspPeq::GstDspPeq(GstAudioFilter *obj)
    : Glib::ObjectBase(typeid(GstDspPeq)),
      Gst::AudioFilter(obj),
      m_filterCount(*this, "filter-count", 1)
{
    set_in_place(true);
    GST_BASE_TRANSFORM_GET_CLASS(Gst::BaseTransform::gobj())->transform_ip = &transform_ip;

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

GstFlowReturn GstDspPeq::transform_ip(GstBaseTransform* self, GstBuffer* buf)
{
    Glib::ObjectBase *const obj_base = static_cast<Glib::ObjectBase*>(Glib::ObjectBase::_get_current_wrapper((GObject*)self));
    GstDspPeq *const obj = dynamic_cast<GstDspPeq* const>(obj_base);
    if (obj) // This can be NULL during destruction.
    {
        // Call the virtual member method, which derived classes might override.
        obj->process(buf);
    }

    return GST_FLOW_OK;
}

void GstDspPeq::process(GstBuffer* buf)
{
    GstMapInfo map;
    gst_buffer_map(buf, &map, (GstMapFlags)GST_MAP_READWRITE);

    float* data = (float*)(map.data);
    uint   sampleCount = map.size/sizeof(float);

    m_mutex.lock();
    for (auto& biquad : m_biquads) {
        biquad->update();
        biquad->process(data, sampleCount);
    }
    m_mutex.unlock();

    gst_buffer_unmap(buf, &map);
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
