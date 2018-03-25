#ifndef GSTDSPBIQUAD_H
#define GSTDSPBIQUAD_H

#include <vector>

#include <gstreamermm.h>
#include <gstreamermm/private/object_p.h>

#include "common/Types.h"

class GstDspBiquad : public Gst::Element
{
public:
    explicit GstDspBiquad(GstElement *obj);

    static void class_init(Gst::ElementClass<GstDspBiquad> *klass);

    void update();
    void process(float* in, uint sampleCount);

    Glib::PropertyProxy<FilterType> type();
    Glib::PropertyProxy<double> freq();
    Glib::PropertyProxy<double> gain();
    Glib::PropertyProxy<double> q();

    void setAudioInfo(const Gst::AudioInfo& info);

private:
    /// Types
    struct ChannelHistory {
        float x1 = 0.0, x2 = 0.0;
        float y1 = 0.0, y2 = 0.0;
    };

    /// Functions
    void onPropertyChanged();

    /// Variables
    bool m_dirty = true;

    Glib::Property<FilterType>   m_type;
    Glib::Property<double> m_freq;
    Glib::Property<double> m_gain;
    Glib::Property<double> m_q;

    BiQuad m_biquad;

    std::vector<ChannelHistory> m_channelHistory;
    int m_rate = 44100;

    std::mutex m_mutex;
};

#endif // GSTDSPBIQUAD_H
