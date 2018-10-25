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
#include <Qt5GStreamer/QGst/Pipeline>

class QDBusObjectPath;

namespace audio
{

class Pipeline : public QObject
{
    Q_OBJECT

public:
    /** Configuration for Pipeline construction. */
    struct Configuration {
        QString transport;
    };

    explicit Pipeline(const Configuration &configuration, QObject *parent = nullptr);
    ~Pipeline();

private:
    QGst::ElementPtr    m_bluetoothSource;
    QGst::PipelinePtr   m_pipeline;
};

} // namespace audio
