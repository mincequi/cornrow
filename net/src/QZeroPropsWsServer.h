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

#pragma once

#include <QtWebSockets/QWebSocketServer>
#include "QZeroPropsServerPrivate.h"

namespace QtZeroProps
{
class QZeroPropsWsService;

class QZeroPropsWsServer : public QtZeroProps::QZeroPropsServerPrivate
{
    Q_OBJECT

public:
    explicit QZeroPropsWsServer(QObject *parent = nullptr);
    ~QZeroPropsWsServer() override;

private:
    virtual QZeroPropsServicePrivate* createService(const ServiceConfiguration& config) override;

    QWebSocketServer    m_server;

    friend class QZeroPropsServer;
};

} // namespace net
