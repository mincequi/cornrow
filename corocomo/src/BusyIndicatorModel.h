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

#include <complex>
#include <random>

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QVector2D>

class BusyIndicatorModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool active MEMBER m_active WRITE setActive)
    Q_PROPERTY(QVector2D center MEMBER m_center)
    Q_PROPERTY(double radius MEMBER m_radius)
    Q_PROPERTY(uint numPoints MEMBER m_numPoints)
    Q_PROPERTY(std::vector<double> xCoords READ xCoords NOTIFY coordsChanged)
    Q_PROPERTY(std::vector<double> yCoords READ yCoords NOTIFY coordsChanged)

public:
    // TODO(mawe): Currently, this class is not used as singleton. However, this is likely to come.
    //             So, we keep the default constructor public and also keep the singleton impl.
    BusyIndicatorModel(QObject *parent = nullptr);
    static BusyIndicatorModel* instance();

    void setActive(bool active);
    Q_INVOKABLE void setRhoDeviation(double dev);
    Q_INVOKABLE void setThetaDeviation(double dev);
    std::vector<double> xCoords() const;
    std::vector<double> yCoords() const;

signals:
    void coordsChanged();

private:
    void randomize();
    double randRho();
    double randTheta();
    std::complex<double> polar(double rho, double theta);

    static BusyIndicatorModel* s_instance;

    bool        m_active = false;
    QVector2D   m_center = {60, 60};
    double      m_radius = 36.0;
    uint        m_numPoints = 7;
    std::vector<double>  m_xCoords;
    std::vector<double>  m_yCoords;

    std::random_device m_rd;
    std::mt19937 m_gen;
    std::normal_distribution<> m_rhoDist;
    std::normal_distribution<> m_thetaDist;
    //std::uniform_real_distribution<> m_rhoDist;
    //std::uniform_real_distribution<> m_thetaDist;

    QTimer m_updateTimer;
    QTimer m_randTimer;
};

Q_DECLARE_METATYPE(QVector<QVector2D>)
