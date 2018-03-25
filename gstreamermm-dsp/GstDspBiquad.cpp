#include "GstDspBiquad.h"

#include <assert.h>
#include <cmath>
#include <iostream>

GstDspBiquad::GstDspBiquad(GstElement *obj)
    : Glib::ObjectBase(typeid(GstDspBiquad)),
      Gst::Element(obj),
      m_type(*this, "type", FilterType::Peak),
      m_freq(*this, "freq", 1000.0),
      m_gain(*this, "gain", 0.0),
      m_q(*this, "q", 0.707)
{
    type().signal_changed().connect(sigc::mem_fun(*this, &GstDspBiquad::onPropertyChanged));
    freq().signal_changed().connect(sigc::mem_fun(*this, &GstDspBiquad::onPropertyChanged));
    gain().signal_changed().connect(sigc::mem_fun(*this, &GstDspBiquad::onPropertyChanged));
    q().signal_changed().connect(sigc::mem_fun(*this, &GstDspBiquad::onPropertyChanged));
}

void GstDspBiquad::class_init(Gst::ElementClass<GstDspBiquad> *klass)
{
    klass->set_metadata("Biquad filter",
                        "Filter/Effect/Audio",
                        "A biquad filter",
                        "Manuel Weichselbaumer <mincequi@web.de>");
}

void GstDspBiquad::update()
{
    if (!m_dirty) return;
    if (m_q == 0.0) return;
    if (m_freq == 0.0) return;

    m_mutex.lock();
    m_dirty = false;
    m_mutex.unlock();

    switch (m_type) {
    case FilterType::Peak: {
        double A = pow(10, m_gain/40.0);
        double w0 = 2*M_PI*m_freq/m_rate;
        double alpha = sin(w0)*0.5/m_q;

        double alpha1 = alpha*A;
        double alpha2 = alpha/A;
        double a0     = 1.0 + alpha2;

        m_biquad.b0 = ( 1.0 + alpha1 ) / a0;
        m_biquad.b1 = (-2.0 * cos(w0)) / a0;
        m_biquad.b2 = ( 1.0 - alpha1 ) / a0;
        m_biquad.a1 = m_biquad.b1;
        m_biquad.a2 = ( 1.0 - alpha2 ) / a0;

        break;
    }
    case FilterType::Invalid:
    case FilterType::Max:
        m_freq = 0.0;
        m_gain = 0.0;
        m_q = 0.0;
    }
}

void GstDspBiquad::process(float* in, uint sampleCount)
{
    if (m_gain == 0.0) return;
    if (m_q == 0.0) return;
    if (m_freq == 0.0) return;

    // Ch1|Ch2|Ch3|Ch4 || Ch1|Ch2|Ch3|Ch4 || Ch1|Ch2|Ch3|Ch4    -> 4 Channels, 3 frames -> 12 samples
    const uint frameCount = sampleCount/m_channelHistory.size();

    for (uint i = 0; i < frameCount; ++i) {
        for (auto& c : m_channelHistory) {
            float out;
            out = m_biquad.b0**in + m_biquad.b1*c.x1 + m_biquad.b2*c.x2 - m_biquad.a1*c.y1 - m_biquad.a2*c.y2;
            c.y2 = c.y1;
            c.y1 = out;
            c.x2 = c.x1;
            c.x1 = *in;

            *in = out;

            ++in;
        }
    }
}

Glib::PropertyProxy<FilterType> GstDspBiquad::type()
{
    return m_type.get_proxy();
}

Glib::PropertyProxy<double> GstDspBiquad::freq()
{
    return m_freq.get_proxy();
}

Glib::PropertyProxy<double> GstDspBiquad::gain()
{
    return m_gain.get_proxy();
}

Glib::PropertyProxy<double> GstDspBiquad::q()
{
    return m_q.get_proxy();
}

void GstDspBiquad::setAudioInfo(const Gst::AudioInfo& info)
{
    if (m_channelHistory.size() != static_cast<std::size_t>(info.get_channels())) {
        m_channelHistory.resize(info.get_channels());
    }

    if (m_rate != info.get_rate()) {
        m_rate = info.get_rate();
        m_mutex.lock();
        m_dirty = true;
        m_mutex.unlock();
    }
}

void GstDspBiquad::onPropertyChanged()
{
    m_mutex.lock();
    m_dirty = true;
    m_mutex.unlock();
}
