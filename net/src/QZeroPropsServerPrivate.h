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

#pragma once

#include <QObject>

namespace QtZeroProps
{
class QZeroPropsService;
class QZeroPropsServicePrivate;
struct ServiceConfiguration;

class QZeroPropsServerPrivate : public QObject
{
    Q_OBJECT
public:
    explicit QZeroPropsServerPrivate(QObject *parent = nullptr);
    ~QZeroPropsServerPrivate();

    // @TODO(mawe): rename
    QZeroPropsService* startPublishing(const ServiceConfiguration& config);
    virtual void stopService();

protected:
    virtual QZeroPropsServicePrivate* createService(const ServiceConfiguration& config) = 0;

    QZeroPropsService* m_currentService = nullptr;
};

} // namespace QtZeroProps
