#include "Controller.h"

#include "config/Controller.h"

Controller::Controller(QObject *parent)
{
    // Create objects
    m_bluetooth = new bluetooth::Controller(this);
    m_audio = new audio::Controller(this);
    m_config = new config::Controller(this);

    connect(m_bluetooth, &bluetooth::Controller::configurationSet, this, &Controller::onBluetoothConnected);
    connect(m_bluetooth, &bluetooth::Controller::configurationCleared, this, &Controller::onBluetoothDisconnected);
}

Controller::~Controller()
{
}

void Controller::onBluetoothConnected(const QDBusObjectPath &transportObjectPath)
{
    m_audio->setTransport(transportObjectPath);
    m_config->setAudioController(m_audio);
}

void Controller::onBluetoothDisconnected(const QDBusObjectPath &transportObjectPath)
{
    m_config->unsetAudioController();
    m_audio->clearTransport();
}
