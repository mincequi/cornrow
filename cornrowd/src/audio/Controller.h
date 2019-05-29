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

#include "Pipeline.h"

#include <common/Types.h>

#include <QObject>

namespace audio
{

class Controller : public QObject
{
    Q_OBJECT

public:
    explicit Controller(QObject *parent = nullptr);
    ~Controller();

    void setTransport(const std::string& transport);
    void clearTransport();

    std::vector<common::Filter> filters(common::FilterGroup group);
    void setFilters(common::FilterGroup group, const std::vector<common::Filter>& filters);

private:
    void updatePipeline();

    Pipeline* m_pipeline = nullptr;

    std::string m_transport;
    std::map<common::FilterGroup, std::vector<common::Filter>> m_filters;
};

} // namespace audio
