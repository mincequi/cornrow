#include "GstDspPlugin.h"

#include <iostream>

#include <gstreamermm/elementfactory.h>
#include <gstreamermm/init.h>
#include <gstreamermm/register.h>

#include "GstDspCrossover.h"
#include "GstDspLoudness.h"
#include "GstDspPeq.h"

GstDspPlugin::GstDspPlugin()
{
}

bool GstDspPlugin::registerPlugin(Glib::RefPtr<Gst::Plugin> plugin)
{
    bool success = true;

    success = success && Gst::ElementFactory::register_element(plugin, "biquad",    GST_RANK_NONE, Gst::register_mm_type<GstDspBiquad>("biquad"));
    success = success && Gst::ElementFactory::register_element(plugin, "loudness",  GST_RANK_NONE, Gst::register_mm_type<GstDspLoudness>("loudness"));
    auto peqType = Gst::register_mm_type<GstDspPeq>("peq");
    GstDspPeq::add_interface(peqType);
    success = success && Gst::ElementFactory::register_element(plugin, "peq",       GST_RANK_NONE, peqType);
    success = success && Gst::ElementFactory::register_element(plugin, "crossover", GST_RANK_NONE, Gst::register_mm_type<GstDspCrossover>("crossover"));

    return success;
}
