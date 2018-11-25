#include "BusyIndicatorModel.h"

#include <QDebug>

BusyIndicatorModel::BusyIndicatorModel(QObject *parent) :
    QObject(parent),
    m_gen(m_rd()),
    m_rhoDist(0.0, 0.2*m_radius),
    m_thetaDist(0.0, 0.0*M_PI/m_numPoints)
    //m_rhoDist(-24.0, +24.0),
    //m_thetaDist(-M_PI/m_numPoints, M_PI/m_numPoints)
{
    m_xCoords.resize(m_numPoints);
    m_yCoords.resize(m_numPoints);

    randomize();

    connect(&m_randTimer, &QTimer::timeout, this, &BusyIndicatorModel::randomize);
}

void BusyIndicatorModel::setActive(bool active)
{
    if (active) {
        m_randTimer.start(150);
    } else {
        m_randTimer.stop();
    }
}

void BusyIndicatorModel::setRhoDeviation(double dev)
{
    m_rhoDist = std::normal_distribution<>(0.0, dev*m_radius);
}

void BusyIndicatorModel::setThetaDeviation(double dev)
{
    m_thetaDist = std::normal_distribution<>(0.0, dev*M_PI/m_numPoints);
}

void BusyIndicatorModel::randomize()
{
    for (uint i = 0; i < m_numPoints; ++i) {
        // @TODO(mawe): android 4.4 does not have std::polar. So, using own impl.
        //std::complex<double> coord = std::polar(abs(m_radius+randRho()), (i*M_PI*2.0/m_numPoints)+randTheta());
        std::complex<double> coord = polar(abs(m_radius+randRho()), (i*M_PI*2.0/m_numPoints)+randTheta());
        m_xCoords[i] = m_center.x() + coord.real();
        m_yCoords[i] = m_center.y() + coord.imag();
    }
    emit coordsChanged();
}

std::vector<double> BusyIndicatorModel::xCoords() const
{
    return m_xCoords;
}

std::vector<double> BusyIndicatorModel::yCoords() const
{
    return m_yCoords;
}

double BusyIndicatorModel::randRho()
{
    auto rho = m_rhoDist(m_gen);
    if (rho > 0.0) {
        rho *= 0.5;
    }
    return rho;
}

double BusyIndicatorModel::randTheta()
{
    return m_thetaDist(m_gen);
}

std::complex<double> BusyIndicatorModel::polar(double rho, double theta)
{
    return std::complex<double>(rho * cos(theta), rho * sin(theta));
}
