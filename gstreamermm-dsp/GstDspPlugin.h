#ifndef GSTDSPPLUGIN_H
#define GSTDSPPLUGIN_H

#include <gstreamermm/plugin.h>

class GstDspPlugin
{
public:
    GstDspPlugin();

    static bool registerPlugin(Glib::RefPtr<Gst::Plugin> plugin);
};

#endif // GSTDSPPLUGIN_H
