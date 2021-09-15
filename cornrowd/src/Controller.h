#pragma once

#include <QObject>

#include "Config.h"
#include "audio/AudioManager.h"
#include "bluetooth/Controller.h"
#include "config/ConfigManager.h"

namespace QtZeroProps {
class QZeroPropsServer;
}

class Controller : public QObject {
    Q_OBJECT

public:
    Controller(const Config& config, QObject* parent = nullptr);
    ~Controller();

private:
    void onTransportChanged(int fd, uint16_t blockSize, uint32_t sampleRate);
    void onVolumeChanged(float volume);
    void onClientDisconnected();

    audio::AudioManager*      m_audio;
    bluetooth::Controller*  m_bluetoothService;
    QtZeroProps::QZeroPropsServer* m_zpServer;
    QtZeroProps::QZeroPropsService* m_zpService;
    config::ConfigManager*     m_config;
};
