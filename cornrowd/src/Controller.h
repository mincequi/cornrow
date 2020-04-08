#pragma once

#include <QObject>

#include "audio/Controller.h"
#include "bluetooth/Controller.h"
#include "config/Controller.h"

namespace net {
class QZeroPropsWsServer;
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

    audio::Controller*      m_audio;
    bluetooth::Controller*  m_bluetoothService;
    net::QZeroPropsWsServer*         m_tcpServer;
    config::Controller*     m_config;
};
