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

    m_peq = Glib::RefPtr<GstDspPeq>::cast_static(Gst::ElementFactory::create_element("peq", "peq0"));
}

GstDspWrapper::~GstDspWrapper()
{
}

bool GstDspWrapper::createPipeline(const Config& config)
{
    if (m_pipeline) destroyPipeline();
    if (config.rate == 0) {
        std::cout << __func__ << ": no signal, not creating pipeline" << std::endl;
        return false;
    }

#ifdef __linux__
    auto src = Gst::AlsaSrc::create();
    auto sink = Gst::AlsaSink::create();
#else
    auto src = Gst::ElementFactory::create_element("autoaudiosrc");
    auto sink = Gst::ElementFactory::create_element("autoaudiosink");
#endif

    auto srcConvert = Gst::AudioConvert::create();
    auto sinkConvert = Gst::AudioConvert::create();

    //auto crossover = Gst::ElementFactory::create_element("crossover");

    std::cout << __func__ << ": creating pipeline...";
    m_pipeline = Gst::Pipeline::create("cornrow-pipeline");
    m_pipeline->add(src)->add(srcConvert)->add(m_peq)->add(sinkConvert)->add(sink);
    std::cout << "created" << std::endl;
    Glib::ustring capsString = Glib::ustring::compose(
                                   "audio/x-raw, "
                                   "format=(string)%1, "
                                   "rate=(int)%2, "
                                   "channels=(int)2, "
                                   "layout=(string)interleaved",
                                   GST_AUDIO_NE(S16),
                                   config.rate);
    auto caps = Gst::Caps::create_from_string(capsString);

    try {
        std::cout << __func__ << ": linking pipeline...";
        src->link(srcConvert, caps)->link(m_peq)->link(sinkConvert)->link(sink, caps);
        std::cout << "linked" << std::endl;
    } catch (...) {
        std::cerr << std::endl << __func__ << ": link error" << std::endl;
        return false;
    }

    std::cout << __func__ << ": starting pipeline...";
    m_pipeline->set_state(Gst::STATE_PLAYING);
    Gst::State state, pending;
    Gst::StateChangeReturn ret = m_pipeline->get_state(state, pending, Gst::CLOCK_TIME_NONE);
    if (ret != Gst::STATE_CHANGE_SUCCESS) {
        std::cerr << std::endl << __func__ << ": start error" << std::endl;
        return false;
    }

    std::cout << "started" << std::endl;
    return true;
}

void GstDspWrapper::destroyPipeline()
{
    /*
    Gst::State state, pending;
    std::cout << __func__ << ": getting pipeline state..." << std::flush;
    Gst::StateChangeReturn ret = m_pipeline->get_state(state, pending, Gst::CLOCK_TIME_NONE);
    std::cout << state << std::endl;
    if (state != Gst::STATE_NULL) {
        std::cout << __func__ << ": stopping pipeline..." << std::flush;
        m_pipeline->set_state(Gst::STATE_NULL);
        std::cout << "stopped" << std::endl;
    }
    */

    m_pipeline->remove(m_peq);
    delete(m_pipeline.release());
}

void GstDspWrapper::setPassthrough(bool passthrough)
{
    cout << "passthrough: " << passthrough << endl;
    m_peq->set_passthrough(passthrough);
}

std::vector<::Preset> GstDspWrapper::getPresets()
{
}

::Preset GstDspWrapper::getPreset(const std::string& name)
{
}

std::vector<std::string> GstDspWrapper::getPresetNames()
{
}

void GstDspWrapper::setPreset(const std::string& name)
{
}

void GstDspWrapper::savePreset(const ::Preset& preset)
{
}

void GstDspWrapper::renamePreset(const std::string& oldName, const std::string& newName)
{
}

void GstDspWrapper::deletePreset(const std::string& name)
{
}

void GstDspWrapper::setFilterCount(uint8_t i)
{
    m_peq->filterCount().set_value(i);
}

void GstDspWrapper::setFilterType(uint8_t i, FilterType type)
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
