#include "Model.h"

#include <QPen>

Model::Model(const std::vector<float>& freqTable,
             const std::vector<float>& qTable,
             QObject *parent) :
    QObject(parent),
    m_freqTable(freqTable),
    m_qTable(qTable)
{
}

void Model::addFilter()
{
    m_filters.append(Filter());
    emit filterCountChanged();

    setFilter(m_filters.size()-1);
}

void Model::deleteFilter()
{
    m_filters.removeAt(m_curIndex);
    setFilter(m_curIndex);

    emit filterCountChanged();
}

void Model::setFilter(int i)
{
    if (m_filters.empty()) {
        m_curFilter = nullptr;
        m_curIndex = -1;
    } else {
        if (i >= m_filters.size()) {
            m_curIndex = m_filters.size()-1;
        } else {
            m_curIndex = i;
        }
        m_curFilter = &(m_filters[m_curIndex]);

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

FilterType Model::type() const
{
    return m_curFilter->t;
}

void Model::setType(FilterType type)
{
    m_curFilter->t = type;
}

float Model::freq() const
{
    return m_freqTable.at(m_curFilter->f);
}

void Model::stepFreq(int i)
{
    int idx = m_freqTable.size() + i;
    if (idx < 0) return;
    if (idx > m_freqTable.size()-1) return;
    if (m_curFilter->f == idx) return;

    m_curFilter->f = idx;

    emit freqChanged();
    emit freqSliderChanged();
}

float Model::freqSlider() const
{
    return (float)m_curFilter->f/(float)(m_freqTable.size()-1);
}

void Model::setFreqSlider(float f)
{
    int idx = qRound(f*(m_freqTable.size()-1));
    if (m_curFilter->f == idx) return;

    m_curFilter->f = idx;
    emit freqChanged();
}

float Model::gain() const
{
    return m_curFilter->g;
}

void Model::setGain(float g)
{
    if (m_curFilter->g == g) return;

    m_curFilter->g = g;
    emit gainChanged();
}

float Model::q() const
{
    return m_qTable.at(m_curFilter->q);
}

void Model::stepQ(int i)
{
    int idx = m_qTable.size() + i;
    if (idx < 0) return;
    if (idx > m_qTable.size()-1) return;
    if (m_curFilter->q == idx) return;

    m_curFilter->q = idx;

    emit qChanged();
    emit qSliderChanged();
}

float Model::qSlider() const
{
    return (float)m_curFilter->q/(float)(m_qTable.size()-1);
}

void Model::setQSlider(float q)
{
    int idx = qRound(q*(m_qTable.size()-1));
    if (m_curFilter->q == idx) return;

    m_curFilter->q = idx;
    emit qChanged();
}
