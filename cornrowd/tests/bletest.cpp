#include <assert.h>
#include <iostream>

#include <gstreamermm.h>
#include <gstreamermm-dsp.h>
#include <glibmm/main.h>

#include <Crossover.h>
#include <coro/audio/Peq.h>

int main(int argc, char** argv)
{
    assert(coro::init());

    // Create mainloop and pipeline
    auto mainloop = Glib::MainLoop::create();
    assert(mainloop);
    auto pipeline = Gst::Pipeline::create();
    assert(pipeline);

    // Create source
    auto src = Gst::AudioTestSrc::create();
    assert(src);
    src->property_wave().set_value(Gst::AudioTestSrcWave::AUDIO_TEST_SRC_WAVE_WHITE_NOISE);

    // Create equalizer
    Glib::RefPtr<coro::audio::Peq> peq = Glib::RefPtr<coro::audio::Peq>::cast_dynamic(Gst::ElementFactory::create_element("peq"));
    assert(peq);
    peq->biquad(0).setFilter( { coro::FilterType::Peak, 1000.0, -12.0, 0.707 } );

    // Create crossover
    Glib::RefPtr<coro::Crossover> xo = Glib::RefPtr<coro::Crossover>::cast_dynamic(Gst::ElementFactory::create_element("crossover"));
    assert(xo);
    //xo->set_property("frequency", 2000.0);
    //xo->set_property("lfe", true);

    // Create encoder
    auto enc = Gst::ElementFactory::create_element("avenc_ac3");
    assert(enc);
    enc->set_property("bitrate", 640000);

    // Create sink
    auto sink = Gst::AlsaSink::create();
    assert(sink);

    // Link elements
    pipeline->add(src)->add(peq)->add(xo)->add(enc)->add(sink);
    src->link(peq)->link(xo)->link(enc)->link(sink);

    // Handle messages posted on bus
    pipeline->get_bus()->add_watch([mainloop, pipeline] (const Glib::RefPtr<Gst::Bus>&, const Glib::RefPtr<Gst::Message>& message) {
        switch (message->get_message_type()) {
        case Gst::MESSAGE_EOS:
        case Gst::MESSAGE_ERROR:
            pipeline->set_state(Gst::STATE_NULL);
            mainloop->quit();
            break;
        default:
            break;
        }
        return true;
    });

    // Start
    pipeline->set_state(Gst::STATE_PLAYING);
    mainloop->run();

    return 0;
}
