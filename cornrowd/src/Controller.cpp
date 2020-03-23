#include "Controller.h"

#include "config/Controller.h"
#include <net/NetService.h>

#include <QDBusObjectPath>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    // Create objects
    m_bluetoothService = new bluetooth::Controller(this);
    m_audio = new audio::Controller(this);
    m_config = new config::Controller(m_audio, m_bluetoothService, this);

    connect(m_bluetoothService, &bluetooth::Controller::transportChanged, this, &Controller::onTransportChanged);
    connect(m_bluetoothService, &bluetooth::Controller::volumeChanged, this, &Controller::onVolumeChanged);

    m_netService = new net::NetService(m_audio, this);
}

Controller::~Controller()
{
    m_config->writeConfig();
}

void Controller::onTransportChanged(int fd, uint16_t blockSize, uint32_t sampleRate)
{
    m_config->writeConfig();
    m_audio->setTransport(fd, blockSize, sampleRate);
}

void Controller::onVolumeChanged(float volume)
{
    m_audio->setVolume(volume);
}
