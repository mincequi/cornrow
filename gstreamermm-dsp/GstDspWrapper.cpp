#include "GstDspWrapper.h"

#include <iostream>

#include <gstreamermm.h>

#include "GstDspPeq.h"
#include "GstDspPlugin.h"

GstDspWrapper::GstDspWrapper()
{
    Gst::init();
    if (!Gst::Plugin::register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
                                      "dsp", "Plugin offering DSP functionalities",
                                      sigc::ptr_fun(&GstDspPlugin::registerPlugin),
                                      "0.1.0", "Proprietary", "", "", "")) {
        std::cerr << "Error registering plugin" << std::endl;
    }

    m_pipeline = Gst::Pipeline::create("cornrow-pipeline");
    auto source = Gst::AudioTestSrc::create();
    source->property_wave().set_value(Gst::AUDIO_TEST_SRC_WAVE_PINK_NOISE);
    //auto source = Gst::ElementFactory::create_element("alsasrc");
    m_peq = Glib::RefPtr<GstDspPeq>::cast_dynamic(Gst::ElementFactory::create_element("peq", "peq0"));
    //auto biquad = Gst::ElementFactory::create_element("biquad", "biquad1");
    //auto crossover = Gst::ElementFactory::create_element("crossover");
    auto sink   = Gst::ElementFactory::create_element("autoaudiosink");

    if (!m_pipeline || !source || !sink || !m_peq) {
        std::cerr << "unable to create element" << std::endl;
    }

    m_pipeline->add(source)->add(sink)->add(m_peq); //->add(loudness)->add(crossover);
    source->link(m_peq)->link(sink);
    //source->link(sink);
    m_pipeline->set_state(Gst::STATE_PLAYING);

    std::cerr << "GstDspWrapper constructed" << std::endl;
}

GstDspWrapper::~GstDspWrapper()
{
}

void GstDspWrapper::setPassthrough(bool passthrough)
{
    m_peq->set_passthrough(passthrough);
}

void GstDspWrapper::setFilterCount(uint8_t i)
{
    m_peq->filterCount().set_value(i);
}

void GstDspWrapper::setFilterType(uint8_t i, Type type)
{
    increaseFilterCount(i+1);
    m_peq->biquads()[i]->type().set_value(type);
}

void GstDspWrapper::setFilterFreq(uint8_t i, float f)
{
    increaseFilterCount(i+1);
    m_peq->biquads()[i]->freq().set_value(f);
}

void GstDspWrapper::setFilterGain(uint8_t i, float g)
{
    increaseFilterCount(i+1);
    m_peq->biquads()[i]->gain().set_value(g);
}

void GstDspWrapper::setFilterQ(uint8_t i, float q)
{
    increaseFilterCount(i+1);
    m_peq->biquads()[i]->q().set_value(q);
}

void GstDspWrapper::increaseFilterCount(uint8_t i)
{
    if (m_peq->filterCount() < i) {
        m_peq->filterCount().set_value(i);
    }
}
