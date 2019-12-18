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

#include "AudioSinkAgent.h"

#include <QtDBus/QDBusObjectPath>

#include <BluezQt/Device>

#include <loguru/loguru.hpp>

namespace bluetooth
{

using namespace BluezQt;

class AudioSinkAgentPrivate
{
public:
    QStringList allowedUuids;
};

AudioSinkAgent::AudioSinkAgent(const QStringList &uuids, QObject *parent)
    : Agent(parent)
    , d(new AudioSinkAgentPrivate)
{
    d->allowedUuids = uuids;
}

AudioSinkAgent::~AudioSinkAgent()
{
    delete d;
}

QDBusObjectPath AudioSinkAgent::objectPath() const
{
    return QDBusObjectPath("/Agent/FifoAgent");
}

Agent::Capability AudioSinkAgent::capability() const
{
    return Agent::NoInputNoOutput;
}

void AudioSinkAgent::authorizeService(DevicePtr device, const QString &uuid, const Request<> &request)
{
    if (!d->allowedUuids.contains(uuid)) {
        request.reject();
    } else {
        LOG_F(INFO, "Service %s accepted from %s", uuid.toStdString().c_str(), device->friendlyName().toStdString().c_str());
        request.accept();
    }
}

} // namespace bluetooth
