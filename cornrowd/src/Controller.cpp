#include "Controller.h"

#include "config/Controller.h"

#include <QDBusObjectPath>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    // Create objects
    m_bluetooth = new bluetooth::Controller(this);
    m_audio = new audio::Controller(this);
    m_config = new config::Controller(m_audio, m_bluetooth, this);

    connect(m_bluetooth, &bluetooth::Controller::transportChanged, this, &Controller::onTransportChanged);
    connect(m_bluetooth, &bluetooth::Controller::volumeChanged, this, &Controller::onVolumeChanged);
}

Controller::~Controller()
{
    m_config->writeConfig();
}

void Controller::onTransportChanged(int fd, uint16_t imtu, uint16_t omtu)
{
    m_audio->setTransport(fd, imtu, omtu, 44100);
}

void Controller::onVolumeChanged(float volume)
{
    m_audio->setVolume(volume);
}
