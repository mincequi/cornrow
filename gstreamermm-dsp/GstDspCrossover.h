#ifndef GSTDSPCROSSOVER_H
#define GSTDSPCROSSOVER_H

#include <gstreamermm.h>
#include <gstreamermm/private/audiofilter_p.h>

class GstDspCrossover : public Gst::AudioFilter
{
public:
    explicit GstDspCrossover(GstAudioFilter *obj);

    static void class_init(Gst::ElementClass<GstDspCrossover> *klass);
};

#endif // GSTDSPCROSSOVER_H
