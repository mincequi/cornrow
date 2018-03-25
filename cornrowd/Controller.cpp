#include "Controller.h"

#include <functional>

#include "protocol/v1/ServerAdapter.h"
#include "protocol/v2/ServerAdapter.h"

#define FUNC(code) std::string(1, static_cast<uint8_t>(code))

Controller::Controller()
    : m_server(0),
      m_mainloop(Glib::MainLoop::create())
{
    m_server.bind(FUNC(v1::Code::Login), [this](uint8_t version, std::string password) { return onLogin(static_cast<Version>(version), password); });
    m_server.async_run();

    m_zeroconf.announce(m_server.get_port());

    m_mainloop->run();
}

Controller::~Controller()
{
    if (m_adapter) {
        delete m_adapter;
        m_adapter = nullptr;
    }
}

bool Controller::onLogin(Version version, std::string /*password*/)
{
    if (m_adapter) {
        m_server = rpc::server(m_server.get_port());
        m_server.bind(FUNC(v1::Code::Login), [this](uint8_t version, std::string password) { return onLogin(static_cast<Version>(version), password); });
        m_server.async_run();
        delete m_adapter;
        m_adapter = nullptr;
    }

    switch (version) {
    case Version1:
        m_adapter = new v1::ServerAdapter(m_server, m_gst);
        return true;
        break;
    case Version2:
        m_adapter = new v2::ServerAdapter(m_server, m_gst);
        return true;
        break;
    default:
        break;
    }

    return false;
}
