#pragma once

#include <QObject>

#include "audio/AudioManager.h"
#include "bluetooth/Controller.h"
#include "config/Controller.h"

namespace QtZeroProps {
class QZeroPropsServer;
}

class Controller : public QObject
{
    Q_OBJECT

public:
    Controller(QObject* parent = nullptr);
    ~Controller();

private:
    void onTransportChanged(int fd, uint16_t blockSize, uint32_t sampleRate);
    void onVolumeChanged(float volume);
    void onClientDisconnected();

    audio::AudioManager*      m_audio;
    bluetooth::Controller*  m_bluetoothService;
    QtZeroProps::QZeroPropsServer* m_zpServer;
    QtZeroProps::QZeroPropsService* m_zpService;
    config::Controller*     m_config;
};
