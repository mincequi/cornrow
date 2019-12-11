#include "Plot.h"

#include <complex>

#include <common/Util.h>

#include "Config.h"

Plot::Plot(const Config& config) :
    m_config(config)
{
    m_magSum.resize(m_config.freqTable.size());
    m_phaseSum.resize(m_config.freqTable.size());

    for (uint i = 0; i < m_config.freqTable.size(); ++i) {
        m_magSum[i].rx() = i;
        m_phaseSum[i].rx() = i;
    }
}

Plot& Plot::operator=(const Plot&)
{
    return *this;
}

void Plot::setFilter(const common::Filter& filter)
{
    switch (filter.type) {
    case common::FilterType::Invalid:
        resizePolys(0, m_mags);
        resizePolys(0, m_phases);
        break;
    case common::FilterType::Peak:
    case common::FilterType::LowPass:
    case common::FilterType::HighPass:
    case common::FilterType::LowShelf:
    case common::FilterType::HighShelf:
    case common::FilterType::AllPass:
        resizePolys(1, m_mags);
        resizePolys(1, m_phases);
        computeResponse(filter, m_mags.front(), m_phases.front());
        break;
    case common::FilterType::Loudness: {
        resizePolys(3, m_mags);
        resizePolys(3, m_phases);
        computeResponse({common::FilterType::Peak, 35.5, filter.g*0.3, 0.56}, m_mags[0], m_phases[0]);
        computeResponse({common::FilterType::Peak, 100.0, filter.g*0.225, 0.25}, m_mags[1], m_phases[1]);
        computeResponse({common::FilterType::HighShelf, 10000.0, filter.g*0.225, 0.80}, m_mags[2], m_phases[2]);
        for (int i = 0; i < m_mags.front().size(); ++i) {
            m_magSum[i].ry() = m_mags.at(0).at(i).y() + m_mags.at(1).at(i).y() + m_mags.at(2).at(i).y();
        }
        for (int i = 0; i < m_phases.front().size(); ++i) {
            m_phaseSum[i].ry() = m_phases.at(0).at(i).y() + m_phases.at(1).at(i).y() + m_phases.at(2).at(i).y();
        }
        double volume = (filter.g*-0.425);
        for (auto& p : m_magSum) { p.ry() += volume; };
        m_mags = {m_magSum};
        m_phases = {m_phaseSum};
        break;
    }
    case common::FilterType::CrossoverLr4:
    case common::FilterType::CrossoverLr2:
    case common::FilterType::Subwoofer:
        resizePolys(2, m_mags);
        resizePolys(2, m_phases);

        double lowGain = filter.g > 0.0 ? -filter.g : 0.0;
        computeResponse({common::FilterType::LowPass, filter.f, lowGain, filter.q}, m_mags[0], m_phases[0]);
        if (filter.type == common::FilterType::CrossoverLr4) {
            for (auto& p : m_mags[0]) { p.ry() += p.ry(); }
            for (auto& p : m_phases[0]) { p.ry() += p.ry(); }
        } // multi pass for cascading
        // Apply gain
        if (lowGain < 0.0) for (auto& p : m_mags[0]) { p.ry() += lowGain; }

        double highGain = filter.g < 0.0 ? filter.g : 0.0;
        computeResponse({common::FilterType::HighPass, filter.f, highGain, filter.q}, m_mags[1], m_phases[1]);
        if (filter.type == common::FilterType::CrossoverLr4) {
            for (auto& p : m_mags[1]) { p.ry() += p.ry(); }
            for (auto& p : m_phases[1]) { p.ry() += p.ry(); }
        } // multi pass for cascading
        // Apply gain
        if (highGain < 0.0) for (auto& p : m_mags[1]) { p.ry() += highGain; }

        sumMags(m_mags, m_magSum);
        sumPhases(m_phases, m_phaseSum);
        break;
    }
}

void Plot::setFrequencyIndex(uchar i)
{
    m_f = i;
}

const QList<QPolygonF>& Plot::mags() const
{
    return m_mags;
}

const QPolygonF& Plot::magSum() const
{
    if (m_mags.empty()) {
        static const QPolygonF emptyPoly;
        return emptyPoly;
    } else if (m_mags.size() == 1) {
        return m_mags.first();
    } else {
        return m_magSum;
    }
}

const QList<QPolygonF>& Plot::phases() const
{
    return m_phases;
}

const QPolygonF& Plot::phaseSum() const
{
    if (m_phases.empty()) {
        static const QPolygonF emptyPoly;
        return emptyPoly;
    } else if (m_phases.size() == 1) {
        return m_phases.first();
    } else {
        return m_phaseSum;
    }
}

void Plot::resizePolys(int newSize, QList<QPolygonF>& polys)
{
    int diff = newSize - polys.size();
    if (diff > 0) {
        polys.reserve(diff);
        while (diff--) {
            QPolygonF poly(m_config.freqTable.size());
            for (uint i = 0; i < m_config.freqTable.size(); ++i) {
                poly[i].rx() = i;
            }

            polys.append(poly);
        }
    } else if (diff < 0) {
        polys.erase(polys.end() + diff, polys.end());
    }
}

void Plot::computeResponse(const common::Filter& f, QPolygonF& mags, QPolygonF& phases) const
{
    common::BiQuad biquad;
    if (!computeBiQuad(44100, f, &biquad)) {
        for (QPointF& m : mags) {
            m.ry() = 0.0;
        }
        return;
    }

    auto m = mags.begin();
    auto p = phases.begin();
    for (; m != mags.end() && p != phases.end(); ) {
        double w = 2.0*M_PI*m_config.freqTable.at(m->rx())/44100;
        std::complex<double> z(cos(w), sin(w));
        std::complex<double> numerator = biquad.b0 + (biquad.b1 + biquad.b2*z)*z;
        std::complex<double> denominator = 1.0 + (biquad.a1 + biquad.a2*z)*z;
        std::complex<double> res = numerator / denominator;
        m->ry() = 20.0*log10(abs(res));
        p->ry() = std::atan2(std::imag(res), std::real(res));
        ++m; ++p;
    }
}

void Plot::sumMags(const QList<QPolygonF>& levels, QPolygonF& sum)
{
    for (int i = 0; i < sum.size(); ++i) {
        qreal level = 0.0;
        for (const auto& l : levels) {
            level += pow(10, (l.at(i).y()/20.0));
        }
        sum[i].ry() = 20*log10(level);
    }
}

void Plot::sumPhases(const QList<QPolygonF>& phases, QPolygonF& sum)
{
    for (int i = 0; i < m_f; ++i) {
        sum[i].ry() = phases.at(0).at(i).y();
    }
    for (int i = m_f; i < sum.size(); ++i) {
        sum[i].ry() = phases.at(1).at(i).y();
    }
}
