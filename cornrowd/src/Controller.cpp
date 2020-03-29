#include "Controller.h"

#include "config/Controller.h"
#include <net/TcpServer.h>

#include <QDBusObjectPath>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    // Remote services
    m_bluetoothService = new bluetooth::Controller(this);
    m_tcpServer = new net::TcpServer(this);

    m_audio = new audio::Controller(this);
    m_config = new config::Controller(m_audio, m_bluetoothService, m_tcpServer, this);

    connect(m_bluetoothService, &bluetooth::Controller::transportChanged, this, &Controller::onTransportChanged);
    connect(m_bluetoothService, &bluetooth::Controller::volumeChanged, this, &Controller::onVolumeChanged);
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
