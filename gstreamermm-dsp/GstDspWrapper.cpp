#include "GstDspWrapper.h"

#include <iostream>

#include <gstreamermm.h>

#include "GstDspPeq.h"
#include "GstDspPlugin.h"

using namespace std;

GstDspWrapper::GstDspWrapper()
{
    Gst::init();
    if (!Gst::Plugin::register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
                                      "dsp", "Plugin offering DSP functionalities",
                                      sigc::ptr_fun(&GstDspPlugin::registerPlugin),
                                      "0.1.0", "Proprietary", "", "", "")) {
        cerr << "Error registering plugin" << std::endl;
    }

    m_pipeline = Gst::Pipeline::create("cornrow-pipeline");

#ifdef __linux__
    auto source = Gst::AlsaSrc::create();
    auto sink = Gst::AlsaSink::create();
#else
    auto source = Gst::ElementFactory::create_element("autoaudiosrc");
    auto sink = Gst::ElementFactory::create_element("autoaudiosink");
#endif

    m_peq = Glib::RefPtr<GstDspPeq>::cast_dynamic(Gst::ElementFactory::create_element("peq", "peq0"));
    //auto crossover = Gst::ElementFactory::create_element("crossover");

    auto convert1 = Gst::AudioConvert::create();
    auto convert2 = Gst::AudioConvert::create();

    if (!m_pipeline || !source || !sink || !m_peq) {
        std::cerr << "unable to create element" << std::endl;
    }

    m_pipeline->add(source)->add(sink)->add(m_peq)->add(convert1)->add(convert2); //->add(loudness)->add(crossover);

    Glib::ustring capsString = Glib::ustring::compose(
                "audio/x-raw, "
                "format=(string)%1, "
                "rate=(int){44100,48000}, "
                "channels=(int)2, "
                "layout=(string)interleaved", GST_AUDIO_NE(S16));
    auto caps = Gst::Caps::create_from_string(capsString);

    source->link(convert1)->link(m_peq)->link(convert2)->link(sink, caps);
    m_pipeline->set_state(Gst::STATE_PLAYING);
}

GstDspWrapper::~GstDspWrapper()
{
}

void GstDspWrapper::setPassthrough(bool passthrough)
{
    cout << "passthrough: " << passthrough << endl;
    m_peq->set_passthrough(passthrough);
}

void GstDspWrapper::setFilterCount(uint8_t i)
{
    m_peq->filterCount().set_value(i);
}

void GstDspWrapper::setFilterType(uint8_t i, Type type)
{
    cout << "setFilterType[" << static_cast<uint32_t>(i) << "]: " << static_cast<uint32_t>(type) << endl;
    increaseFilterCount(i+1);
    m_peq->biquads()[i]->type().set_value(type);
}

void GstDspWrapper::setFilterFreq(uint8_t i, float f)
{
    cout << "setFilterFreq[" << static_cast<uint32_t>(i) << "]: " << f << endl;
    increaseFilterCount(i+1);
    m_peq->biquads()[i]->freq().set_value(f);
}

void GstDspWrapper::setFilterGain(uint8_t i, float g)
{
    cout << "setFilterGain[" << static_cast<uint32_t>(i) << "]: " << g << endl;
    increaseFilterCount(i+1);
    m_peq->biquads()[i]->gain().set_value(g);
}

void GstDspWrapper::setFilterQ(uint8_t i, float q)
{
    cout << "setFilterQ[" << static_cast<uint32_t>(i) << "]: " << q << endl;
    increaseFilterCount(i+1);
    m_peq->biquads()[i]->q().set_value(q);
}

void GstDspWrapper::increaseFilterCount(uint8_t i)
{
    if (m_peq->filterCount() < i) {
        m_peq->filterCount().set_value(i);
    }
}
