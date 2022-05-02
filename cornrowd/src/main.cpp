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
#include <QLoggingCategory>

#include <loguru/loguru.hpp>

#include "Config.h"
#include "Controller.h"

struct SignalHandler {
    SignalHandler() {
        signal(SIGINT, &QCoreApplication::exit);
        signal(SIGTERM, &QCoreApplication::exit);
        signal(SIGSTOP, &QCoreApplication::exit);
    }
};

int main(int argc, char **argv) {
    /* Everything with a verbosity equal or greater than g_stderr_verbosity will be
    written to stderr. You can set this in code or via the -v argument.
    Set to loguru::Verbosity_OFF to write nothing to stderr.
    Default is 0, i.e. only log ERROR, WARNING and INFO are written to stderr.
    */
    loguru::g_stderr_verbosity  = 0;
    loguru::g_colorlogtostderr  = true; // True by default.
    loguru::g_flush_interval_ms = 0; // 0 (unbuffered) by default.
    loguru::g_preamble          = true; // Prefix each log line with date, time etc? True by default.

    /* Specify the verbosity used by loguru to log its info messages including the header
    logged when logged::init() is called or on exit. Default is 0 (INFO).
    */
    loguru::g_internal_verbosity = 1;

    loguru::g_preamble_date    = false;
    loguru::g_preamble_time    = true; // The time of the current day
    loguru::g_preamble_uptime  = false; // The time since init call
    loguru::g_preamble_thread  = true; // The logging thread
    loguru::g_preamble_file    = true; // The file from which the log originates from
    loguru::g_preamble_verbose = true; // The verbosity field
    loguru::g_preamble_pipe    = true; // The pipe symbol right before the message

    loguru::init(argc, argv);

    SignalHandler signalHandler;

    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("cornrowd");
    QCoreApplication::setApplicationVersion("0.8.1");

    // Parse command line options
    QCommandLineParser parser;
    parser.setApplicationDescription("cornrowd is a software DSP service with Bluetooth audio sink and Bluetooth LE control capabilities.");
    parser.addVersionOption();
    parser.addHelpOption();
    QCommandLineOption configFileOption({"c", "config"},
                                        "Set configuration file to <file>.",
                                        "file");
    parser.addOption(configFileOption);
    parser.process(a);
    const auto configFile = parser.value(configFileOption).toStdString();

    // Command line config
    Config config(configFile);
    config.parse();

    // suppress some qt bluetooth warnings
    QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth.bluez.warning=false"));

    new Controller(config, &a);

    return a.exec();
}
