#pragma once

#include <QObject>

#include <common/Types.h>
#include <common/ble/Types.h>

#include "Config.h"

class BleCentralAdapter;
class IoModel;
class PresetModel;

namespace ble {
class BleClient;
}

namespace net {
class TcpClient;
}

class FilterModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int peqFilterCount READ peqFilterCount CONSTANT)

    Q_PROPERTY(int currentBand READ currentBand WRITE setCurrentBand NOTIFY currentBandChanged)
    Q_PROPERTY(std::vector<bool> activeFilters READ activeFilters NOTIFY filterTypeChanged)

    Q_PROPERTY(uint filterType READ filterType WRITE setFilterType NOTIFY filterTypeChanged)
    Q_PROPERTY(QStringList filterTypeNames READ filterTypeNames NOTIFY currentBandChanged)

    Q_PROPERTY(double freqSlider READ freqSlider WRITE setFreqSlider NOTIFY freqSliderChanged)
    Q_PROPERTY(QString freqReadout READ freqReadout NOTIFY freqChanged)

    Q_PROPERTY(double gainSlider READ gainSlider WRITE setGainSlider NOTIFY gainSliderChanged)
    Q_PROPERTY(double gain READ gain NOTIFY gainChanged)

    Q_PROPERTY(double qSlider READ qSlider WRITE setQSlider NOTIFY qSliderChanged)
    Q_PROPERTY(QString qReadout READ qReadout NOTIFY qChanged)

public:
    static FilterModel* init(const Config& configuration, net::TcpClient* tcpClient, ble::BleClient* bleClient);
    static FilterModel* instance();

    Q_INVOKABLE void resizeFilters(int diff);

    int         peqFilterCount() const;

    int         currentBand() const;
    Q_INVOKABLE void setCurrentBand(int i);

    std::vector<bool> activeFilters() const;

    int         filterType() const;
    void        setFilterType(int filterType);
    QStringList filterTypeNames() const;

    QString     freqReadout() const;
    Q_INVOKABLE void stepFreq(int i);
    double      freqSlider() const;
    void        setFreqSlider(double);

    double      gain() const;
    Q_INVOKABLE void stepGain(int i);
    double      gainSlider() const;
    void        setGainSlider(double g);
    double      gainMin() const;
    double      gainMax() const;
    double      gainStep() const;

    QString     qReadout() const;
    Q_INVOKABLE void stepQ(int i);
    double      qSlider() const;
    void        setQSlider(double);

    // Crossover related members
    // int         crossoverType() const;
    // void        setCrossoverType(int filterType);
    QStringList crossoverTypeNames() const;

    int         subwooferType() const;
    void        setSubwooferType(int filterType);

signals:
    void currentBandChanged();
    void filterChanged(int i, uchar t, double f, double g, double q);
    void filterTypeChanged();
    void freqChanged();
    void freqSliderChanged();
    void gainChanged();
    void gainSliderChanged();
    void qChanged();
    void qSliderChanged();

private:
    // This is the model-oriented filter struct. We use indexed values here.
    struct Filter {
        explicit Filter(common::FilterType t, uint8_t f, double g, uint8_t q);
        common::FilterType t;
        uint8_t f;
        double  g;
        uint8_t q;
    };

    FilterModel(const Config& config, net::TcpClient* tcpClient, ble::BleClient* bleClient);

    static uint8_t snap(double value, uint8_t min, uint8_t max, uint8_t step);

    void onFilterChangedLocally();
    void onFilterChangedRemotely(const QUuid& key, const QByteArray& value);

    static FilterModel* s_instance;

    const Config& m_config;

    QList<Filter>   m_filters;
    Filter*         m_currentFilter = nullptr;
    int             m_currentBand = 0;
    const int       m_loudnessBand;
    const int       m_xoBand;
    const int       m_scBand;
    double          m_freqSlider;

    net::TcpClient* m_tcpClient = nullptr;
    ble::BleClient* m_bleClient = nullptr;
};
