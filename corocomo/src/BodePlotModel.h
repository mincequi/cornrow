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

#include <QObject>

#include "Plot.h"

class BodePlotModel : public QObject
{
    Q_OBJECT

public:
    static BodePlotModel* instance();

    Q_INVOKABLE void setFilter(int i, uchar t, uchar f, double g, double q);
    //Q_INVOKABLE void setCrossover(uchar t, uchar f, double g, double q);
    //Q_INVOKABLE void setSubwoofer(uchar t, uchar f, double g, double q);

    const QList<Plot>& plots() const;

private:
    BodePlotModel(QObject* parent = nullptr);

    static BodePlotModel* s_instance;

    std::vector<double> m_freqTable;

    QList<Plot>  m_graphs;

    uchar m_f = 0;
};
