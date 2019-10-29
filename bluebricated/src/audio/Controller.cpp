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

#include <QDebug>
#include <QThread>
#include <QtDBus/QDBusObjectPath>

#include <gstreamermm-dsp.h>
#include <common/Types.h>

#include "Pipeline.h"

#include <unistd.h>

namespace audio
{

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    // Init gstreamermm-dsp
    GstDsp::init();

    m_pipeline = new Pipeline(Pipeline::Type::Normal);;
}

Controller::~Controller()
{
    delete m_pipeline;
}

void Controller::setTransport(int fd, uint16_t blockSize, int rate)
{
    // Stop pipeline
    m_pipeline->setTransport(-1, 0, 0);

    m_fd = fd;
    m_blockSize = blockSize;
    m_rate = rate;

    m_pipeline->setTransport(m_fd, m_blockSize, m_rate);
}

void Controller::setVolume(float volume)
{
    m_pipeline->setVolume(volume);
}

} // namespace audio
