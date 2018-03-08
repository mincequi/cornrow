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
}
