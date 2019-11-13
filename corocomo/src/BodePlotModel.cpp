#include "BodePlotModel.h"

#include "Config.h"
#include "Plot.h"

BodePlotModel* BodePlotModel::s_instance = nullptr;

BodePlotModel* BodePlotModel::instance()
{
    return s_instance;
}

BodePlotModel* BodePlotModel::init(const Config& configuration)
{
    if (s_instance) {
        return s_instance;
    }

    s_instance = new BodePlotModel(configuration);
    return s_instance;
}

BodePlotModel::BodePlotModel(const Config& config, QObject *parent) :
    QObject(parent),
    m_config(config)
{
}

void BodePlotModel::setFilter(int idx, uchar t, uchar f, double g, double q)
{
    m_f = f;

    // Dynamically append graphs
    int diff = idx-m_graphs.size()+1;
    while (diff-- > 0) {
        Plot graph(m_config);
        m_graphs.append(graph);
    }

    // Apply filter parameters
    m_graphs[idx].setFrequencyIndex(f);
    m_graphs[idx].setFilter({static_cast<common::FilterType>(t), m_config.freqTable.at(f), g, q});
}

/*
void BodePlotModel::setCrossover(int idx,
                                 uchar xoT, uchar xoF, double xoG, double xoQ,
                                 uchar swT, uchar swF, double swG, double swQ)
{
    int diff = idx-m_graphs.size()+1;
    while (diff-- > 0) {
        Plot graph(m_config);
        m_graphs.append(graph);
    }

    // Apply filter parameters
    m_graphs[idx].setFrequencyIndex(f);
    m_graphs[idx].setFilter({static_cast<common::FilterType>(t), m_config.freqTable.at(f), g, q});
}
*/

const QList<Plot>& BodePlotModel::plots() const
{
    return m_graphs;
}
