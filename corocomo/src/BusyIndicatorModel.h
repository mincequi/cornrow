#pragma once

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
    BusyIndicatorModel(QObject *parent = nullptr);

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

    bool        m_active = false;
    QVector2D   m_center = {60, 60};
    double      m_radius = 36.0;
    uint        m_numPoints = 11;
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
