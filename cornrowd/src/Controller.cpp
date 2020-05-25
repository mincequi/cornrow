#include "Controller.h"

#include "config/Controller.h"
#include <QtZeroProps/QZeroPropsServer.h>
#include <QtZeroProps/QZeroPropsService.h>
#include <QtZeroProps/QZeroPropsTypes.h>

#include <QDBusObjectPath>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    // Remote services
    m_bluetoothService = new bluetooth::Controller(this);
    m_zpServer = new QtZeroProps::QZeroPropsServer(this);
    m_zpService = m_zpServer->startService( { "_cornrow._tcp" } );

    m_audio = new audio::Controller(this);
    m_config = new config::Controller(m_audio, m_bluetoothService, m_zpService, this);

    connect(m_bluetoothService, &bluetooth::Controller::transportChanged, this, &Controller::onTransportChanged);
    connect(m_bluetoothService, &bluetooth::Controller::volumeChanged, this, &Controller::onVolumeChanged);
    connect(m_zpServer, &QtZeroProps::QZeroPropsServer::clientDisconnected, this, &Controller::onClientDisconnected);
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

void Controller::onClientDisconnected()
{
    m_config->writeConfig();
}
