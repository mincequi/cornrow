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

#include "NetService.h"

#include <QCoreApplication>
#include <QDebug>
#include <QHostInfo>
#include <QThread>
#include <QTimer>
#include <QtEndian>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtRemoteObjects/QRemoteObjectHost>

#include <qzeroconf.h>

#include <loguru/loguru.hpp>
#include <zeroeq/publisher.h>
#include <zeroeq/server.h>
#include <zeroeq/subscriber.h>

#include <cmath>
#include <unistd.h>

using namespace std::placeholders;

namespace net
{

NetService::NetService(QObject *parent)
    : QObject(parent)
{
    // Open service
    QTcpServer* server = new QTcpServer(this);
    if (!server->listen(QHostAddress::AnyIPv4)) {
        LOG_F(ERROR, "Error starting NetService.");
        return;
    }

    m_host = new QRemoteObjectHost(this);
    QObject::connect(server, &QTcpServer::newConnection, m_host, [&]() {
        m_host->addHostSideConnection(server->nextPendingConnection());
    });
    m_host->enableRemoting(new Filter(this), "Cornrow");

    // Publish service
    QZeroConf* zeroConf = new QZeroConf(this);
    connect(zeroConf, &QZeroConf::servicePublished, [&]() {
        LOG_F(INFO, "Cornrow server published at port: %i", server->serverPort());
    });
    connect(zeroConf, &QZeroConf::error, [](QZeroConf::error_t error) {
        LOG_F(INFO, "Error publishing service: %i", error);
    });
    zeroConf->startServicePublish(QHostInfo::localHostName().toStdString().c_str(),
                                  "_cornrow._tcp",
                                  nullptr,
                                  qToBigEndian(server->serverPort()));

    //m_publisher = new zeroeq::Publisher();
    //QByteArray data = "1234";
    //m_publisher->publish(zeroeq::uint128_t(1234), data.data(), data.size());

    /*
    m_server = new zeroeq::Server("cornrow");
    LOG_F(INFO, "SERVER session: %s", m_server->getSession().c_str());

    m_server->handle(zeroeq::uint128_t(1234), [](const void* _data, size_t size) {
        QByteArray data((char *)_data, size);
        std::cerr << "REQUEST received: " << data.toStdString() << std::endl;
        return [&](zeroeq::uint128_t(1234), _data, size) {};
    });

    const zeroeq::HandleFunc handleFunc = [&](const void* _data, size_t size) -> zeroeq::ReplyData {
        QByteArray data((char *)_data, size);
        std::cerr << "REQUEST received: " << data.toStdString() << std::endl;
        return { servus::uint128_t(1234), servus::Serializable::Data() };
    };
    m_server->handle(zeroeq::uint128_t(1234), handleFunc);
    */
}

NetService::~NetService()
{
}

void NetService::setReadFiltersCallback(ReadFiltersCallback callback)
{
    m_readFiltersCallback = callback;
}

void NetService::setReadIoCapsCallback(ReadIoCapsCallback callback)
{
    m_readIoCapsCallback = callback;
}

void NetService::setReadIoConfCallback(ReadIoConfCallback callback)
{
    m_readIoConfCallback = callback;
}

QByteArray NetService::onReadPeqFilters()
{
    return m_converter.filtersToBle(m_readFiltersCallback(common::ble::CharacteristicType::Peq));
}

QByteArray NetService::onReadAuxFilters()
{
    return m_converter.filtersToBle(m_readFiltersCallback(common::ble::CharacteristicType::Aux));
}

QByteArray NetService::onReadIoCaps()
{
    return m_converter.toBle(m_readIoCapsCallback());
}

QByteArray NetService::onReadIoConf()
{
    return m_converter.toBle(m_readIoConfCallback());
}

void NetService::onWriteFilters(common::ble::CharacteristicType group, const QByteArray& value)
{
    emit filtersWritten(group, m_converter.filtersFromBle(value));
}

void NetService::onWriteIoConf(const QByteArray& value)
{
    auto interfaces = m_converter.fromBle(value);

    common::IoInterface input;
    common::IoInterface output;

    for (const auto& interface : interfaces) {
        if (interface.isOutput) {
            output = interface;
        } else {
            input = interface;
        }
    }

    emit inputSet(input);
    emit outputSet(output);
}

} // namespace net
