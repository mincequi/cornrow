#include "Model.h"

Model::Model(const std::vector<float>& freqTable, QObject *parent) :
    QObject(parent),
    m_freqTable(freqTable)
{
}

void Model::addFilter()
{
    m_filters.append(Filter());
}

void Model::deleteFilter()
{
}

void Model::setFilter(int i)
{
}

void Model::stepFreq(int i)
{
}

void Model::stepQ(int i)
{
}

int Model::filterCount() const
{
}

FilterType Model::filterType() const
{
}

void Model::setFilterType(FilterType type)
{
}

float Model::freq() const
{
}

float Model::gain() const
{
}

void Model::setGain(float g)
{
}

float Model::q() const
{
}

float Model::freqSlider() const
{
}

void Model::setFreqSlider(float)
{
}

float Model::qSlider() const
{
}

void Model::setQSlider(float q)
{
}
