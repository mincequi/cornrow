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

#include <QtZeroProps/QZeroPropsClient.h>
#include <QtZeroProps/QZeroPropsService.h>

#include <QMap>
#include <QSet>
#include <QTimer>

namespace QtZeroProps
{

class QZeroPropsServicePrivate : public QObject
{
    Q_OBJECT

public:
    QZeroPropsServicePrivate(QZeroPropsService* _q);
    virtual ~QZeroPropsServicePrivate();

    virtual void connect() = 0;
    virtual void disconnect() = 0;

    void onTimeout();
    virtual void doSend(const QVariant& uuid, const QByteArray& value) = 0;

    QTimer          timer;
    QSet<QVariant>  dirtyProperties;
    QMap<QVariant, QByteArray>  properties;

    QString     name;
    QZeroPropsService::ServiceType type = QZeroPropsService::ServiceType::Invalid;

    class QZeroPropsService* const q;

signals:
    void stateChanged(QZeroPropsClient::State state, const QString& errorString = QString());
};

} // namespace QZeroProps
