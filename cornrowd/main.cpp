#include <functional>
#include <iostream>

#include <glibmm/main.h>

#include <rpc/server.h>
#include <rpc/this_handler.h>

#include "gstreamermm-dsp/GstDspWrapper.h"
#include "protocol/ProtocolAdaptersV1.h"
#include "zeroconf/ZeroconfWrapper.h"

using namespace std;

int main()
{
    // Start server with arbitrary port
    rpc::server server(0);
    auto port = server.get_port();

    auto mainloop = Glib::MainLoop::create();
    GstDspWrapper gst;

    v1::ServerProtocolAdapter h(server, gst);
    server.async_run();

    ZeroconfWrapper zeroconf(port);

    cout << "port: " << port << endl;

    mainloop->run();

    return 0;
}
