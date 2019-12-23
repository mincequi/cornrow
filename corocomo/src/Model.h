#ifndef MODEL_H
#define MODEL_H

#include <QObject>

#include <common/Types.h>
#include <common/ble/Types.h>

#include "Config.h"

class BleCentralAdapter;
class IoModel;
class PresetModel;
namespace ble
{
class Client;
}

class Model : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusLabel READ statusLabel NOTIFY statusChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)

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
    enum Status : uint8_t
    {
        NoBluetooth,
        Discovering,
        Connecting,
        Connected,
        Timeout,
        Lost,
        Error
    };
    Q_ENUM(Status)

    static Model* init(const Config& configuration);
    static Model* instance();

    Q_INVOKABLE void startDiscovering();
    Q_INVOKABLE void startDemoMode();

    Status      status() const;
    QString     statusLabel() const;
    QString     statusText() const;

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
    void statusChanged();
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
    Model(const Config& config, QObject* parent = nullptr);

    // This is the model-oriented filter struct. We use indexed values here.
    struct Filter {
        explicit Filter(common::FilterType t, uint8_t f, double g, uint8_t q);
        common::FilterType t;
        uint8_t f;
        double  g;
        uint8_t q;
    };

    void setFilters(common::ble::CharacteristicType group, const std::vector<Filter>& filters);

    void onParameterChanged();
    void onBleStatus(Status status, const QString& errorString);

    static Model* s_instance;

    const Config& m_config;

    Status          m_status = Status::Discovering;
    QString         m_statusLabel = "Discovering";
    QString         m_statusText;
    QList<Filter>   m_filters;
    Filter*         m_currentFilter = nullptr;
    int             m_currentBand = 0;
    const int       m_loudnessBand;
    const int       m_xoBand;
    const int       m_scBand;
    double          m_freqSlider;
    bool            m_demoMode = false;

    // BLE
    ble::Client* m_central = nullptr;
    BleCentralAdapter* m_adapter = nullptr;
    friend class BleCentralAdapter;

    // Sub models
    IoModel* m_ioModel = nullptr;
    PresetModel* m_presetModel = nullptr;
};

#endif // MODEL_H
