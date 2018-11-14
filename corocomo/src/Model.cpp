#include "Model.h"

#include <QPen>

#include <ble/Client.h>

#include "BleCentralAdapter.h"

Model* Model::s_instance = nullptr;

Model* Model::instance()
{
    return s_instance;
}

Model* Model::init(const Config& configuration)
{
    if (s_instance) {
        return s_instance;
    }

    s_instance = new Model(configuration);
    return s_instance;
}

Model::Model(const Config& configuration, QObject *parent) :
    QObject(parent),
    m_configuration(configuration)
{
    resizeFilters(m_configuration.filterCount);
    setCurrentBand(0);

    connect(this, &Model::typeChanged, this, &Model::onParameterChanged);
    connect(this, &Model::freqChanged, this, &Model::onParameterChanged);
    connect(this, &Model::gainChanged, this, &Model::onParameterChanged);
    connect(this, &Model::qChanged, this, &Model::onParameterChanged);

    m_central = new ble::Client(this);
    m_adapter = new BleCentralAdapter(m_central, this);

    connect(m_adapter, &BleCentralAdapter::initPeq, this, &Model::setFilters);
    connect(m_adapter, &BleCentralAdapter::status, this, &Model::onBleStatus);

    m_central->startDiscovering();
}

Model::Filter::Filter(common::FilterType _t, uint8_t _f, double _g, uint8_t _q)
    : t(_t),
      f(_f),
      g(_g),
      q(_q)
{
}

void Model::startDiscovering()
{
    m_demoMode = false;
    m_central->startDiscovering();
    onBleStatus(Status::Discovering, QString());
}

void Model::startDemoMode()
{
    m_demoMode = true;
    onBleStatus(Status::Connected, QString());
}

Model::Status Model::status() const
{
    return m_status;
}

QString Model::statusReadout() const
{
    return m_statusReadout;
}

QString Model::errorReadout() const
{
    return m_errorReadout;
}

void Model::resizeFilters(int diff)
{
    if (diff > 0) {
        while (diff--) {
            m_filters.append(Filter(common::FilterType::Invalid, m_configuration.freqDefault, 0.0, m_configuration.qDefault));
        }
    } else if (diff < 0) {
        m_filters.erase(m_filters.end() + diff, m_filters.end());
    }
}

void Model::setCurrentBand(int i)
{
    if (m_filters.empty()) {
        m_currentFilter = nullptr;
        if (m_currentBand == -1) return;
        m_currentBand = -1;

        emit currentBandChanged();
    } else {
        if (i >= m_filters.size()) {
            m_currentBand = m_filters.size()-1;
        } else {
            m_currentBand = i;
        }
        m_currentFilter = &(m_filters[m_currentBand]);

        emit currentBandChanged();
        emit typeChanged();
        emit freqChanged();
        emit freqSliderChanged();
        emit gainChanged();
        emit qChanged();
        emit qSliderChanged();
    }
}

int Model::filterCount() const
{
    return m_filters.size();
}

int Model::currentBand() const
{
    return m_currentBand;
}

int Model::type() const
{
    return static_cast<int>(m_currentFilter->t);
}

void Model::setType(int type)
{
    common::FilterType t = static_cast<common::FilterType>(type);
    if (m_currentFilter->t == t) return;

    m_currentFilter->t = t;
    emit typeChanged();
}

QString Model::freqReadout() const
{
    double value = m_configuration.freqTable.at(m_currentFilter->f);

    if (value < 1.0) return QString::number(value, 'f', 2);
    else if (value < 100.0) return QString::number(value, 'f', 1);
    else return QString::number(value, 'f', 0);
}

void Model::stepFreq(int i)
{
    int idx = m_currentFilter->f + i;
    if (idx < 0 || idx > static_cast<int>(m_configuration.freqTable.size()-1) || idx == m_currentFilter->f) {
        return;
    }

    m_currentFilter->f = static_cast<uint8_t>(idx);
    emit freqChanged();
    emit freqSliderChanged();
}

double Model::freqSlider() const
{
    return static_cast<double>(m_currentFilter->f)/(m_configuration.freqTable.size()-1);
}

void Model::setFreqSlider(double f)
{
    uint8_t idx = static_cast<uint8_t>(qRound(f*(m_configuration.freqTable.size()-1)));
    if (m_currentFilter->f == idx) return;

    m_currentFilter->f = idx;
    emit freqChanged();
}

double Model::gain() const
{
    return m_currentFilter->g;
}

void Model::stepGain(int i)
{
    double g = m_currentFilter->g+i*m_configuration.gainStep;
    if (g > m_configuration.gainMax || g < m_configuration.gainMin) {
        return;
    }

    m_currentFilter->g = g;
    emit gainChanged();
}

void Model::setGain(double g)
{
    if (g > m_configuration.gainMax || g < m_configuration.gainMin) {
        return;
    }

    m_currentFilter->g = g;
    emit gainChanged();
}

QString Model::qReadout() const
{
    double value = m_configuration.qTable.at(m_currentFilter->q);

    if (value < 1.0) return QString::number(value, 'f', 2);
    else if (value < 10.0) return QString::number(value, 'f', 1);
    else return QString::number(value, 'f', 0);
}

void Model::stepQ(int i)
{
    int idx = m_currentFilter->q + i;
    if (idx < 0) return;
    if (idx > static_cast<int>(m_configuration.qTable.size())-1) return;
    if (m_currentFilter->q == idx) return;

    m_currentFilter->q = static_cast<uint8_t>(idx);

    emit qChanged();
    emit qSliderChanged();
}

double Model::qSlider() const
{
    return static_cast<double>(m_currentFilter->q)/(m_configuration.qTable.size()-1);
}

void Model::setQSlider(double q)
{
    uint8_t idx = static_cast<uint8_t>(qRound(q*(m_configuration.qTable.size()-1)));
    if (m_currentFilter->q == idx) return;

    m_currentFilter->q = idx;
    emit qChanged();
}

void Model::setFilters(const std::vector<Filter>& filters)
{
    for (uint i = 0; i < filters.size(); ++i) {
        m_filters[i].t = filters.at(i).t;
        m_filters[i].f = filters.at(i).f;
        m_filters[i].g = filters.at(i).g;
        m_filters[i].q = filters.at(i).q;
        emit filterChanged(i, static_cast<uchar>(m_filters[i].t), m_configuration.freqTable.at(m_filters[i].f), m_filters[i].g, m_configuration.qTable.at(m_filters[i].q));
    }

    setCurrentBand(0);
}

void Model::onParameterChanged()
{
    if (m_currentFilter) {
        emit filterChanged(m_currentBand, static_cast<uchar>(m_currentFilter->t), m_configuration.freqTable.at(m_currentFilter->f), m_currentFilter->g, m_configuration.qTable.at(m_currentFilter->q));
    } else {
        emit filterChanged(m_currentBand, 0, 0.0, 0.0, 0.0);
    }

    if (!m_demoMode) {
        m_adapter->setPeqDirty();
    }
}

void Model::onBleStatus(Status _status, const QString& errorString)
{
    m_status = _status;
    m_errorReadout.clear();

    switch (_status) {
    case Status::Discovering:
        m_statusReadout = "Discovering";
        break;
    case Status::Connected:
        m_statusReadout = "";
        break;
    case Status::Timeout:
        m_statusReadout = "Timeout";
        m_errorReadout = "Be sure to be close to a Cornrow device.";
        break;
    case Status::Lost:
        m_statusReadout = "Lost";
        m_errorReadout = "Connection has been interrupted.";
        break;
    case Status::Error:
        m_statusReadout = "Error";
        m_errorReadout = errorString;
        break;
    }

    emit statusChanged();
}
