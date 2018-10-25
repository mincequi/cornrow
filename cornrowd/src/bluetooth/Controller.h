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

class QDBusObjectPath;

namespace bluetooth
{

class Controller : public QObject
{
    Q_OBJECT

public:
    explicit Controller(QObject *parent = nullptr);

signals:
    void configurationSet(const QDBusObjectPath &transportObjectPath);
    void configurationCleared(const QDBusObjectPath &transportObjectPath);

private:
    void onConfigurationSet(const QString& transportObjectPath, const QVariantMap& properties);
    void onConfigurationCleared(const QString& transportObjectPath);
};

} // namespace bluetooth
