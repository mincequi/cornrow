#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <glibmm/main.h>

#include <rpc/server.h>
#include <rpc/this_handler.h>

#include "common/Types.h"
#include "input/hifiberrydigi/RateFileWatcher.h"
#include "gstreamermm-dsp/GstDspWrapper.h"
#include "protocol/v1/ServerAdapter.h"
#include "zeroconf/ZeroconfWrapper.h"

class Controller : public sigc::trackable
{
public:
    Controller(const Config& config);
    ~Controller();

private:
    bool onLogin(Version version, std::string password);
    void onRateChanged(int rate);

    rpc::server                     m_server;
    Glib::RefPtr<Glib::MainLoop>    m_mainloop;
    GstDspWrapper                   m_gst;
    v1::ServerAdapter*              m_adapter = nullptr;
    ZeroconfWrapper                 m_zeroconf;
    RateFileWatcher  m_watcher;
};

#endif // CONTROLLER_H
