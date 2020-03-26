#include "FilterModel.h"

#include <math.h>

#include <algorithm>

#include <QDebug>
#include <QPen>

#include <ble/BleClient.h>
#include <common/RemoteDataStore.h>
#include <common/Types.h>

#include "BleCentralAdapter.h"
#include "IoModel.h"
#include "PresetModel.h"

FilterModel* FilterModel::s_instance = nullptr;

FilterModel* FilterModel::instance()
{
    return s_instance;
}

FilterModel* FilterModel::init(const Config& configuration,
                               common::RemoteDataStore* dataStore)
{
    if (s_instance) {
        return s_instance;
    }

    s_instance = new FilterModel(configuration, dataStore);
    return s_instance;
}

FilterModel::FilterModel(const Config& config, common::RemoteDataStore* dataStore) :
    QObject(nullptr),
    m_config(config),
    m_loudnessBand(config.peqFilterCount),
    m_xoBand(config.peqFilterCount+1),
    m_scBand(config.peqFilterCount+2),
    m_remoteStore(dataStore)
{
    auto filterCount = m_config.peqFilterCount;
    if (m_config.loudnessAvailable) ++filterCount;
    if (m_config.xoAvailable) ++filterCount;
    if (m_config.swAvailable) ++filterCount;
    resizeFilters(filterCount);
    setCurrentBand(0);

    connect(this, &FilterModel::filterTypeChanged, this, &FilterModel::onParameterChanged);
    connect(this, &FilterModel::freqChanged, this, &FilterModel::onParameterChanged);
    connect(this, &FilterModel::gainChanged, this, &FilterModel::onParameterChanged);
    connect(this, &FilterModel::qChanged, this, &FilterModel::onParameterChanged);
}

FilterModel::Filter::Filter(common::FilterType _t, uint8_t _f, double _g, uint8_t _q)
    : t(_t),
      f(_f),
      g(_g),
      q(_q)
{
}

void FilterModel::resizeFilters(int diff)
{
    if (diff > 0) {
        while (diff--) {
            m_filters.append(Filter(common::FilterType::Invalid, m_config.freqDefault, 0.0, m_config.qDefault));
        }
    } else if (diff < 0) {
        m_filters.erase(m_filters.end() + diff, m_filters.end());
    }
}

void FilterModel::setCurrentBand(int i)
{
    m_currentBand = i;
    if (i >= 0) {
        m_currentFilter = &(m_filters[m_currentBand]);
    } else {
        m_currentFilter  = nullptr;
    }

    emit currentBandChanged();

    if (!m_currentFilter) {
        return;
    }

    emit filterTypeChanged();
    emit freqChanged();
    emit freqSliderChanged();
    emit gainChanged();
    emit gainSliderChanged();
    emit qChanged();
    emit qSliderChanged();
}

std::vector<bool> FilterModel::activeFilters() const
{
    std::vector<bool> ret;
    ret.reserve(m_filters.size());

    for (const auto& filter : m_filters) {
        ret.push_back(filter.t != common::FilterType::Invalid);
    }

    return ret;
}

int FilterModel::peqFilterCount() const
{
    return m_config.peqFilterCount;
}

int FilterModel::currentBand() const
{
    return m_currentBand;
}

int FilterModel::filterType() const
{
    if (m_currentBand < 0) {
        return 0;
    } else if (m_currentBand < m_config.peqFilterCount) {
        return static_cast<int>(m_currentFilter->t);
    } else if (m_currentBand == m_loudnessBand) {
        return (m_currentFilter->t == common::FilterType::Invalid) ? 0 : 1;
    } else if (m_currentBand == m_xoBand) {
        if (m_currentFilter->t == common::FilterType::CrossoverLr2) return 1;
        else if (m_currentFilter->t == common::FilterType::CrossoverLr4) return 2;
        else return 0;
    } else if (m_currentBand == m_scBand) {
        return (m_currentFilter->t == common::FilterType::Invalid) ? 0 : 1;
    }

    return 0;
}

void FilterModel::setFilterType(int type)
{
    common::FilterType t = common::FilterType::Invalid;

    if (m_currentBand < m_config.peqFilterCount) {
        t = static_cast<common::FilterType>(type);
    } else if (m_currentBand == m_loudnessBand) {
        t = (type == 0)  ? common::FilterType::Invalid : common::FilterType::Loudness;
    } else if (m_currentBand == m_xoBand) {
        if (type == 1) t = common::FilterType::CrossoverLr2;
        else if (type == 2) t = common::FilterType::CrossoverLr4;
        m_currentFilter->q = type == 1 ? 28 : 34;
        //m_currentFilter->g = type == 1 ? 1.0 : 2.0;
    } else if (m_currentBand == m_scBand) {
        t = (type == 0)  ? common::FilterType::Invalid : common::FilterType::Subwoofer;
        m_currentFilter->q = type == 1 ? 28 : 34;
        //m_currentFilter->g = type == 1 ? 1.0 : 2.0;
    }

    // @TODO(mawe): this might be tweaked a little bit
    // if (m_currentFilter->t == t) return;

    m_currentFilter->t = t;
    emit filterTypeChanged();
}

QStringList FilterModel::filterTypeNames() const
{
    if (m_currentBand < m_config.peqFilterCount) {
        return { "Off", "PK", "LP", "HP", "LS", "HS"/*, "AP"*/ };
    } else if (m_currentBand == m_loudnessBand) {
        return { "Off", "On" };
    } else {
        return { "Off", "LR2", "LR4" };
    }
}

QString FilterModel::freqReadout() const
{
    double value = m_config.freqTable.at(m_currentFilter->f);

    if (value < 1.0) return QString::number(value, 'f', 2);
    else if (value < 100.0) return QString::number(value, 'f', 1);
    else return QString::number(value, 'f', 0);
}

void FilterModel::stepFreq(int i)
{
    int idx = m_currentFilter->f + (i*m_config.freqStep);
    if (idx < m_config.freqMin || idx > m_config.freqMax || idx == m_currentFilter->f) {
        return;
    }

    m_currentFilter->f = static_cast<uint8_t>(idx);
    emit freqChanged();
    emit freqSliderChanged();
}

double FilterModel::freqSlider() const
{
    const double value = static_cast<double>(m_currentFilter->f - m_config.freqMin)/(m_config.freqMax - m_config.freqMin);
    return value;
}

void FilterModel::setFreqSlider(double f)
{
    const uint8_t idx = snap(f, m_config.freqMin, m_config.freqMax, m_config.freqStep);

    if (m_currentFilter->f == idx) return;

    m_currentFilter->f = idx;
    emit freqChanged();
}

double FilterModel::gainSlider() const
{
    return (gain()-gainMin())/(gainMax()-gainMin());
}

void FilterModel::setGainSlider(double g)
{
    double gain = static_cast<int>((gainMax()-gainMin())*g/gainStep())*gainStep()+gainMin();
    if (m_currentFilter->g == gain) return;

    m_currentFilter->g = gain;
    emit gainChanged();
}

double FilterModel::gain() const
{
    return m_currentFilter->g;
}

void FilterModel::stepGain(int i)
{
    double g = m_currentFilter->g+i*gainStep();
    if (g > gainMax() || g < gainMin()) {
        return;
    }

    m_currentFilter->g = g;
    emit gainChanged();
    emit gainSliderChanged();
}

double FilterModel::gainMin() const
{
    if (m_currentBand == m_loudnessBand) return 0.0;
    else if (m_currentBand == m_xoBand) return -12.0;
    else return m_config.gainMin;
}

double FilterModel::gainMax() const
{
    if (m_currentBand == m_loudnessBand) return 40.0;
    else if (m_currentBand == m_xoBand) return 12.0;
    else return m_config.gainMax;
}

double FilterModel::gainStep() const
{
    if (m_currentBand == m_loudnessBand) return 1.0;
    else return m_config.gainStep;
}

QString FilterModel::qReadout() const
{
    double value = common::qTable.at(m_currentFilter->q);

    if (value < 1.0) return QString::number(value, 'f', 3);
    else if (value < 10.0) return QString::number(value, 'f', 2);
    else return QString::number(value, 'f', 1);
}

void FilterModel::stepQ(int i)
{
    int idx = m_currentFilter->q + i*m_config.qStep;
    if (idx < m_config.qMin || idx > m_config.qMax || idx == m_currentFilter->q) {
        return;
    }

    m_currentFilter->q = static_cast<uint8_t>(idx);
    emit qChanged();
    emit qSliderChanged();
}

double FilterModel::qSlider() const
{
    return static_cast<double>(m_currentFilter->q - m_config.qMin) /
            static_cast<double>(m_config.qMax - m_config.qMin);
}

void FilterModel::setQSlider(double q)
{
    uint8_t idx = static_cast<uint8_t>(qRound(q*(m_config.qMax - m_config.qMin)));
    idx += idx%m_config.qStep;
    idx += m_config.qMin;

    if (m_currentFilter->q == idx) return;

    m_currentFilter->q = idx;
    emit qChanged();
}

/*
int Model::crossoverType() const
{
    return (m_filters.at(m_xoBand).t == common::FilterType::Invalid) ? 0 : static_cast<int>(m_filters.at(m_xoBand).g);
}

void Model::setCrossoverType(int filterType)
{
    m_filters[m_xoBand].t = (filterType == 0)  ? common::FilterType::Invalid : common::FilterType::CrossoverLr2;
    m_filters[m_xoBand].q = filterType == 1 ? 28 : 34;
    m_filters[m_xoBand].g = filterType == 1 ? 1.0 : 2.0;
}
*/

QStringList FilterModel::crossoverTypeNames() const
{
    return { "Off", "LR2", "LR4" };
}

int FilterModel::subwooferType() const
{
    return (m_filters.at(m_scBand).t == common::FilterType::Invalid) ? 0 : static_cast<int>(m_filters.at(m_scBand).g);
}

void FilterModel::setSubwooferType(int filterType)
{
    m_filters[m_scBand].t = (filterType == 0)  ? common::FilterType::Invalid : common::FilterType::Subwoofer;
    m_filters[m_scBand].q = filterType == 1 ? 28 : 34;
    m_filters[m_scBand].g = filterType == 1 ? 1.0 : 2.0;
}

void FilterModel::readFilters()
{
    {
        const QByteArray& value = m_remoteStore->peq();
        if (value.size() % 4 == 0) {
            int j = 0;
            for (int i = 0; i < value.size() && i <= 4 * m_config.peqFilterCount; i += 4) {
                m_filters[j].t = static_cast<common::FilterType>(value.at(i));
                m_filters[j].f = static_cast<uint8_t>(value.at(i+1));
                m_filters[j].g = value.at(i+2)*0.5;
                m_filters[j].q = static_cast<uint8_t>(value.at(i+3));
                emit filterChanged(j, static_cast<uchar>(m_filters[j].t), m_filters[j].f, m_filters[j].g, common::qTable.at(m_filters[j].q));
                ++j;
            }
        } else {
            qDebug("Invalid size for filter group");
        }
    } {
        const QByteArray& value = m_remoteStore->aux();
        if (value.size() % 4 == 0) {
            int j = m_config.peqFilterCount;
            for (int i = 0; i < value.size() && i <= 4 * (m_filters.size() - m_config.peqFilterCount); i += 4) {
                m_filters[j].t = static_cast<common::FilterType>(value.at(i));
                m_filters[j].f = static_cast<uint8_t>(value.at(i+1));
                m_filters[j].g = value.at(i+2)*0.5;
                m_filters[j].q = static_cast<uint8_t>(value.at(i+3));
                emit filterChanged(j, static_cast<uchar>(m_filters[j].t), m_filters[j].f, m_filters[j].g, common::qTable.at(m_filters[j].q));
                ++j;
            }
        } else {
            qDebug("Invalid size for filter group");
        }
    }

    // m_filters[i].q = filter.q < m_config.qMin ? m_config.qMin : filter.q > m_config.qMax ? m_config.qMax : filter.q;

    setCurrentBand(0);
}

uint8_t FilterModel::snap(double value, uint8_t min, uint8_t max, uint8_t step)
{
    uint8_t idx = static_cast<uint8_t>(qRound(value * (max - min)));
    const uint8_t rest = idx % step;
    idx -= rest;
    if (rest >= step/2) {
        idx += step;
    }

    return idx + min;
}

void FilterModel::onParameterChanged()
{
    if (m_currentFilter) {
        emit filterChanged(m_currentBand, static_cast<uchar>(m_currentFilter->t), m_currentFilter->f, m_currentFilter->g, common::qTable.at(m_currentFilter->q));
    } else {
        emit filterChanged(m_currentBand, 0, 0, 0.0, 0.0);
    }

    if (m_currentBand < m_config.peqFilterCount) {
        QByteArray value(m_config.peqFilterCount * 4, 0);
        for (int i = 0; i < m_config.peqFilterCount; ++i) {
            value[i*4]   = static_cast<char>(m_filters.at(i).t);
            value[i*4+1] = m_filters.at(i).f;
            value[i*4+2] = static_cast<int8_t>(m_filters.at(i).g * 2.0);
            value[i*4+3] = m_filters.at(i).q;
        }
        m_remoteStore->setPeq(value);
    } else {
        QByteArray value((m_filters.count() - m_config.peqFilterCount) * 4, 0);
        for (int i = 0; i < (m_filters.count() - m_config.peqFilterCount); ++i) {
            value[i*4]   = static_cast<char>(m_filters.at(i + m_config.peqFilterCount).t);
            value[i*4+1] = m_filters.at(i + m_config.peqFilterCount).f;
            value[i*4+2] = static_cast<int8_t>(m_filters.at(i+ m_config.peqFilterCount).g * 2.0);
            value[i*4+3] = m_filters.at(i + m_config.peqFilterCount).q;
        }
        m_remoteStore->setAux(value);
    }
}
