#pragma once

#include <QObject>

#include "audio/Controller.h"
#include "bluetooth/Controller.h"
#include "config/Controller.h"

class Controller : public QObject
{
    Q_OBJECT

public:
    Controller(QObject* parent = nullptr);
    ~Controller();

private:
    void onBluetoothConnected(const QDBusObjectPath &transportObjectPath);
    void onBluetoothDisconnected(const QDBusObjectPath &transportObjectPath);

    audio::Controller*      m_audio;
    bluetooth::Controller*  m_bluetooth;
    config::Controller*     m_config;
};
