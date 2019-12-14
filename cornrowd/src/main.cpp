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

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QLoggingCategory>

#include <loguru/loguru.hpp>

#include "Controller.h"

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
    loguru::g_preamble_date    = false;
    loguru::g_preamble_time    = true; // The time of the current day
    loguru::g_preamble_uptime  = false; // The time since init call
    loguru::g_preamble_thread  = false; // The logging thread
    loguru::g_preamble_file    = true; // The file from which the log originates from
    loguru::g_preamble_verbose = true; // The verbosity field
    loguru::g_preamble_pipe    = true; // The pipe symbol right before the message

    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("cornrowd");
    QCoreApplication::setApplicationVersion("0.7.0");

    // command line options
    QCommandLineParser parser;
    parser.setApplicationDescription("cornrowd is a software DSP service with Bluetooth audio sink and Bluetooth LE control capabilities.");
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(a);

    // suppress some qt bluetooth warnings
    QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth.bluez.warning=false"));

    SignalHandler signalHandler;

    new Controller(&a);

    qDebug() << "Waiting for bluetooth audio source to connect. Ctrl + C to cancel...";

    return a.exec();
}

