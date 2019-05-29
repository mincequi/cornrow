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

Model::Model(const Config& config, QObject *parent) :
    QObject(parent),
    m_config(config),
    m_xoBand(config.peqFilterCount),
    m_swBand(config.peqFilterCount+1)
{
    auto filterCount = m_config.peqFilterCount;
    if (m_config.xoAvailable) ++filterCount;
    if (m_config.swAvailable) ++filterCount;
    resizeFilters(filterCount);
    setCurrentBand(0);

    connect(this, &Model::filterTypeChanged, this, &Model::onParameterChanged);
    connect(this, &Model::freqChanged, this, &Model::onParameterChanged);
    connect(this, &Model::gainChanged, this, &Model::onParameterChanged);
    connect(this, &Model::qChanged, this, &Model::onParameterChanged);

    m_central = new ble::Client(this);
    m_adapter = new BleCentralAdapter(m_central, this);

    connect(m_adapter, &BleCentralAdapter::filtersReceived, this, &Model::setFilters);
    connect(m_adapter, &BleCentralAdapter::status, this, &Model::onBleStatus);
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

QString Model::statusLabel() const
{
    return m_statusLabel;
}

QString Model::statusText() const
{
    return m_statusText;
}

void Model::resizeFilters(int diff)
{
    if (diff > 0) {
        while (diff--) {
            m_filters.append(Filter(common::FilterType::Invalid, m_config.freqDefault, 0.0, m_config.qDefault));
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
    }

    m_currentBand = i;
    m_currentFilter = &(m_filters[m_currentBand]);

    emit currentBandChanged();
    emit filterTypeChanged();
    emit freqChanged();
    emit freqSliderChanged();
    emit gainChanged();
    emit qChanged();
    emit qSliderChanged();
}

std::vector<bool> Model::activeFilters() const
{
    std::vector<bool> ret;
    ret.reserve(m_filters.size());

    for (const auto& filter : m_filters) {
        ret.push_back(filter.t != common::FilterType::Invalid);
    }

    return ret;
}

int Model::peqFilterCount() const
{
    return m_config.peqFilterCount;
}

int Model::currentBand() const
{
    return m_currentBand;
}

int Model::filterType() const
{
    if (m_currentBand < m_config.peqFilterCount) {
        return static_cast<int>(m_currentFilter->t);
    } else if (m_currentBand == m_xoBand) {
        return (m_currentFilter->t == common::FilterType::Invalid) ? 0 : 1;
    } else if (m_currentBand == m_swBand) {
        return (m_currentFilter->t == common::FilterType::Invalid) ? 0 : 1;
    }

    return 0;
}

void Model::setFilterType(int type)
{
    common::FilterType t = common::FilterType::Invalid;

    if (m_currentBand < m_config.peqFilterCount) {
        t = static_cast<common::FilterType>(type);
    } else if (m_currentBand == m_xoBand) {
        t = (type == 0)  ? common::FilterType::Invalid : common::FilterType::Crossover;
    } else if (m_currentBand == m_swBand) {
        t = (type == 0)  ? common::FilterType::Invalid : common::FilterType::Subwoofer;
    }

    if (m_currentFilter->t == t) return;

    m_currentFilter->t = t;
    emit filterTypeChanged();
}

QStringList Model::filterTypeNames() const
{
    if (m_currentBand < m_config.peqFilterCount) {
        return { "Off", "Peaking", "LowPass", "HighPass" };
    } else {
        return { "Off", "LR4" };
    }
}

QString Model::freqReadout() const
{
    double value = m_config.freqTable.at(m_currentFilter->f);

    if (value < 1.0) return QString::number(value, 'f', 2);
    else if (value < 100.0) return QString::number(value, 'f', 1);
    else return QString::number(value, 'f', 0);
}

void Model::stepFreq(int i)
{
    int idx = m_currentFilter->f + i;
    if (idx < 0 || idx > static_cast<int>(m_config.freqTable.size()-1) || idx == m_currentFilter->f) {
        return;
    }

    m_currentFilter->f = static_cast<uint8_t>(idx);
    emit freqChanged();
    emit freqSliderChanged();
}

double Model::freqSlider() const
{
    return static_cast<double>(m_currentFilter->f)/(m_config.freqTable.size()-1);
}

void Model::setFreqSlider(double f)
{
    uint8_t idx = static_cast<uint8_t>(qRound(f*(m_config.freqTable.size()-1)));
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
    double g = m_currentFilter->g+i*m_config.gainStep;
    if (g > m_config.gainMax || g < m_config.gainMin) {
        return;
    }

    m_currentFilter->g = g;
    emit gainChanged();
}

void Model::setGain(double g)
{
    if (g > m_config.gainMax || g < m_config.gainMin) {
        return;
    }

    m_currentFilter->g = g;
    emit gainChanged();
}

QString Model::qReadout() const
{
    double value = m_config.qTable.at(m_currentFilter->q);

    if (value < 1.0) return QString::number(value, 'f', 2);
    else if (value < 10.0) return QString::number(value, 'f', 1);
    else return QString::number(value, 'f', 0);
}

void Model::stepQ(int i)
{
    int idx = m_currentFilter->q + i;
    if (idx < 0) return;
    if (idx > static_cast<int>(m_config.qTable.size())-1) return;
    if (m_currentFilter->q == idx) return;

    m_currentFilter->q = static_cast<uint8_t>(idx);

    emit qChanged();
    emit qSliderChanged();
}

double Model::qSlider() const
{
    return static_cast<double>(m_currentFilter->q)/(m_config.qTable.size()-1);
}

void Model::setQSlider(double q)
{
    uint8_t idx = static_cast<uint8_t>(qRound(q*(m_config.qTable.size()-1)));
    if (m_currentFilter->q == idx) return;

    m_currentFilter->q = idx;
    emit qChanged();
}

void Model::setFilters(common::FilterGroup task, const std::vector<Filter>& filters)
{
    uint i = (task == common::FilterGroup::Peq) ? 0 : m_config.peqFilterCount;
    for (; i < filters.size(); ++i) {
        m_filters[i].t = filters.at(i).t;
        m_filters[i].f = filters.at(i).f;
        m_filters[i].g = filters.at(i).g;
        m_filters[i].q = filters.at(i).q;
        // @TODO(mawe): range check filter parameters (e.g. Q from server might be out of local range).
        emit filterChanged(i, static_cast<uchar>(m_filters[i].t), m_config.freqTable.at(m_filters[i].f), m_filters[i].g, m_config.qTable.at(m_filters[i].q));
    }

    setCurrentBand(0);
}

void Model::onParameterChanged()
{
    if (m_currentFilter) {
        emit filterChanged(m_currentBand, static_cast<uchar>(m_currentFilter->t), m_config.freqTable.at(m_currentFilter->f), m_currentFilter->g, m_config.qTable.at(m_currentFilter->q));
    } else {
        emit filterChanged(m_currentBand, 0, 0.0, 0.0, 0.0);
    }

    if (!m_demoMode) {
        m_adapter->setDirty(m_currentBand < m_config.peqFilterCount ? common::FilterGroup::Peq : common::FilterGroup::Aux);
    }
}

void Model::onBleStatus(Status _status, const QString& statusText)
{
    m_status = _status;
    m_statusText.clear();

    switch (_status) {
    case Status::NoBluetooth:
        m_statusLabel = "Bluetooth disabled";
        m_statusText = "Enable Bluetooth in your device's settings.";
        break;
    case Status::Discovering:
        m_statusLabel = "Discovering";
        m_statusText = statusText;
        break;
    case Status::Connecting:
        m_statusLabel = "Connecting";
        m_statusText = statusText;
        break;
    case Status::Connected:
        m_statusLabel = "";
        break;
    case Status::Timeout:
        m_statusLabel = "Timeout";
        m_statusText = "Be sure to be close to a cornrow device.";
        break;
    case Status::Lost:
        m_statusLabel = "Lost";
        m_statusText = "Connection has been interrupted.";
        break;
    case Status::Error:
        m_statusLabel = "Error";
        m_statusText = statusText;
        break;
    }

    emit statusChanged();
}
