/*
 * Copyright (C) 2020 Manuel Weichselbaumer <mincequi@web.de>
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

#include "NetClient.h"

#include <QDebug>
#include "qzeroconf.h"

namespace net
{

NetClient::NetClient(QObject* parent)
    : QObject(parent)
{
	m_zeroConf = new QZeroConf(this);
	connect(m_zeroConf, &QZeroConf::serviceAdded, this, &NetClient::onServiceAdded);
}

NetClient::~NetClient()
{
}

void NetClient::startDiscovering()
{
    if (m_zeroConf->browserExists()) {
        stopDiscovering();
    }

    m_zeroConf->startBrowser("_cornrow._tcp");
}

void NetClient::stopDiscovering()
{
	m_zeroConf->stopBrowser();
}

void NetClient::onServiceAdded(QZeroConfService service)
{
    NetDevicePtr device(new NetDevice);
    device->name = service->name();
    device->type = NetDevice::DeviceType::TcpIp;
    device->address = service->ip();
    device->port = service->port();

    emit deviceDiscovered(device);
    qDebug() << service;
}

} // namespace net
