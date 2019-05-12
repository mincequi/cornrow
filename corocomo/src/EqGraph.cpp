#include "EqGraph.h"

#include <complex>

#include <common/Util.h>

EqGraph::EqGraph(const std::vector<double>& frequencyTable) :
    m_frequencyTable(frequencyTable )
{
}

EqGraph& EqGraph::operator=(const EqGraph&)
{
    return *this;
}

void EqGraph::setFilter(const common::Filter& filter)
{
    switch (filter.type) {
    case common::FilterType::Invalid:
        resizeGraphs(0);
        break;
    case common::FilterType::Peak:
    case common::FilterType::LowPass:
    case common::FilterType::HighPass:
    case common::FilterType::LowShelf:
    case common::FilterType::HighShelf:
        resizeGraphs(1);
        computeResponse(filter, m_graphs.front());
        break;
    case common::FilterType::Crossover:
    case common::FilterType::Subwoofer:
        resizeGraphs(2);
        computeResponse({common::FilterType::LowPass, filter.f, filter.g, filter.q}, m_graphs.front());
        computeResponse({common::FilterType::HighPass, filter.f, filter.g, filter.q}, m_graphs.back());
        sumResponses(m_graphs, m_sum);
        break;
    }

}

const QList<QPolygonF>& EqGraph::graphs() const
{
    return m_graphs;
}

const QPolygonF& EqGraph::sum() const
{
    if (m_graphs.empty()) {
        static const QPolygonF emptyPoly;
        return emptyPoly;
    } else if (m_graphs.size() == 1) {
        return m_graphs.first();
    } else {
        return m_sum;
    }
}

void EqGraph::resizeGraphs(int newSize)
{
    int diff = newSize - m_graphs.size();
    if (diff > 0) {
        m_graphs.reserve(diff);
        while (diff--) {
            QPolygonF poly(m_frequencyTable.size());
            for (uint i = 0; i < m_frequencyTable.size(); ++i) {
                poly[i].rx() = i;
            }

            m_graphs.append(poly);
        }
    } else if (diff < 0) {
        m_graphs.erase(m_graphs.end() + diff, m_graphs.end());
    }

    if (newSize > 1) {
        m_sum.resize(m_frequencyTable.size());
        for (uint i = 0; i < m_frequencyTable.size(); ++i) {
            m_sum[i].rx() = i;
        }
    }
}

void EqGraph::computeResponse(const common::Filter& f, QPolygonF& response) const
{
    common::BiQuad biquad;
    if (!computeBiQuad(44100, f, &biquad)) {
        for (QPointF& m : response) {
            m.ry() = 0.0;
        }
        return;
    }

    for (QPointF& m : response) {
        double w = 2.0*M_PI*m_frequencyTable.at(m.rx())/44100;
        std::complex<double> z(cos(w), sin(w));
        std::complex<double> numerator = biquad.b0 + (biquad.b1 + biquad.b2*z)*z;
        std::complex<double> denominator = 1.0 + (biquad.a1 + biquad.a2*z)*z;
        std::complex<double> res = numerator / denominator;
        m.ry() = 20.0*log10(abs(res));
    }
}

void EqGraph::sumResponses(const QList<QPolygonF>& levels, QPolygonF& sum)
{
    for (int i = 0; i < sum.size(); ++i) {
        qreal level = 0.0;
        for (const auto& l : levels) {
            level += pow(10, (l.at(i).y()/20.0));
        }
        sum[i].ry() = 20*log10(level);
    }
}
