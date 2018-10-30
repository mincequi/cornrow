/*
 * Copyright (C) 2018 Manuel Weichselbaumer <mincequi@web.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <csignal>

#include <QCoreApplication>
#include <QDebug>
#include <QThread>

#include "Config.h"

#include "audio/Controller.h"
#include <ble/Adapter.h>
#include <ble/Peripheral.h>
#include "bluetooth/Controller.h"

struct SignalHandler
{
    SignalHandler()
    {
        signal(SIGINT, &QCoreApplication::exit);
        signal(SIGTERM, &QCoreApplication::exit);
        signal(SIGSTOP, &QCoreApplication::exit);
    }
};

int main(int argc, char **argv)
{
    SignalHandler signalHandler;
    QCoreApplication a(argc, argv);

    qDebug() << "Waiting for bluetooth audio source to connect. Ctrl + C to cancel...";

    // Create objects
    auto bluetoothController = new bluetooth::Controller();
    auto audioController = new audio::Controller();
    auto bleController = new ble::Peripheral();

    // Read persistence
    readConfig(*audioController);

    // Write persistence, when we quit
    QObject::connect(&a, &QCoreApplication::aboutToQuit, [&]() {
        writeConfig(*audioController);
        return;
    });

    // The stream is transported through unix file descriptors, which cannot be read/write
    // acquired from within same thread. So, AudioController gets another thread.
    auto thread = new QThread();
    audioController->moveToThread(thread);
    thread->start();

    // We inherently need a QueuedConnection here (apparently, an AutoConnection does not work).
    QObject::connect(bluetoothController, &bluetooth::Controller::configurationSet,
                     audioController, &audio::Controller::setTransport, Qt::QueuedConnection);
    QObject::connect(bluetoothController, &bluetooth::Controller::configurationCleared,
                     audioController, &audio::Controller::clearTransport, Qt::QueuedConnection);

    // BLE
    auto adapter = new ble::Adapter(bleController, audioController->peq());
    QObject::connect(adapter, &ble::Adapter::peq, audioController, &audio::Controller::setPeq);

    return a.exec();
}
