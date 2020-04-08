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

#include <QtZeroProps/QZeroPropsService.h>

#include "QZeroPropsWsService.h"

#include <QDebug>
#include <QVariant>

uint qHash(const QVariant& var)
{
    switch (var.type()) {
    case QVariant::Uuid:
        return qHash(var.toUuid());
    case QVariant::UInt:
        return qHash(var.toUInt());
    default:
        return 0;
    }
}

namespace QtZeroProps
{

QZeroPropsService::QZeroPropsService(QObject* parent)
    : QObject(parent)
{
}

QZeroPropsService::~QZeroPropsService()
{
    delete d;
}

QString QZeroPropsService::name() const
{
    return d->name;
}

QZeroPropsService::ServiceType QZeroPropsService::type() const
{
    return d->type;
}

void QZeroPropsService::setDebounceTime(int msec)
{
    d->timer.setInterval(msec);
}

void QZeroPropsService::setProperty(quint32 uuid, const QByteArray& value)
{
    d->properties.insert(uuid, value);

    d->dirtyProperties.insert(uuid);
    if (!d->timer.isActive()) {
        d->timer.start();
    }
}

void QZeroPropsService::setProperty(const QUuid& uuid, const QByteArray& value)
{
    d->properties.insert(uuid, value);

    d->dirtyProperties.insert(uuid);
    if (!d->timer.isActive()) {
        d->timer.start();
    }
}

} // namespace QZeroProps
