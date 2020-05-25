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

#include "QZeroPropsServicePrivate.h"

#include <QDebug>

namespace QtZeroProps
{

QZeroPropsServicePrivate::QZeroPropsServicePrivate(QZeroPropsService* _q)
    : q(_q)
{
    // Setup timer
    timer.setInterval(200);
    timer.setSingleShot(true);
    QTimer::connect(&timer, &QTimer::timeout, [this]() {
        onTimeout();
    });
}

QZeroPropsServicePrivate::~QZeroPropsServicePrivate()
{
}

void QZeroPropsServicePrivate::onTimeout()
{
    // Iterate dirty properties and send them
    for (const auto& key : dirtyProperties) {
        const auto& value = properties[key];
        qDebug() << "Send property:" << key << ", value size:" << value.size();
        doSend(key, value);
    }

    // Clear dirty propertiers
    dirtyProperties.clear();
}

} // namespace QZeroProps
