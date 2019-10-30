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

#include "Controller.h"

#include <csignal>

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QLoggingCategory>

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);

    new Controller(&a);

    qDebug() << "Waiting for bluetooth audio source to connect. Ctrl + C to cancel...";

    return a.exec();
}

