#ifndef MODEL_H
#define MODEL_H

#include <QObject>

#include <common/Types.h>

#include "Config.h"

class BleCentralAdapter;
namespace ble
{
class Central;
}

class Model : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusReadout READ statusReadout NOTIFY statusChanged)
    Q_PROPERTY(QString errorReadout READ errorReadout NOTIFY statusChanged)

    Q_PROPERTY(int filterCount READ filterCount NOTIFY filterCountChanged)
    Q_PROPERTY(int currentBand READ currentBand WRITE setCurrentBand NOTIFY currentBandChanged)

    Q_PROPERTY(uint type READ type WRITE setType NOTIFY typeChanged)

    Q_PROPERTY(double freqSlider READ freqSlider WRITE setFreqSlider NOTIFY freqSliderChanged)
    Q_PROPERTY(QString freqReadout READ freqReadout NOTIFY freqChanged)

    Q_PROPERTY(double gain READ gain WRITE setGain NOTIFY gainChanged)

    Q_PROPERTY(double qSlider READ qSlider WRITE setQSlider NOTIFY qSliderChanged)
    Q_PROPERTY(QString qReadout READ qReadout NOTIFY qChanged)

public:
    enum Status : uint8_t
    {
        Discovering,
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
    QString     statusReadout() const;
    QString     errorReadout() const;

    Q_INVOKABLE void resizeFilters(int diff);

    int         filterCount() const;

    int         currentBand() const;
    Q_INVOKABLE void setCurrentBand(int i);

    int         type() const;
    void        setType(int type);

    QString     freqReadout() const;
    Q_INVOKABLE void stepFreq(int i);
    double      freqSlider() const;
    void        setFreqSlider(double);

    double      gain() const;
    Q_INVOKABLE void stepGain(int i);
    void        setGain(double g);

    QString     qReadout() const;
    Q_INVOKABLE void stepQ(int i);
    double      qSlider() const;
    void        setQSlider(double);

signals:
    void statusChanged();
    void filterCountChanged();
    void currentBandChanged();
    void filterChanged(int i, uchar t, double f, double g, double q);
    void typeChanged();
    void freqChanged();
    void freqSliderChanged();
    void gainChanged();
    void qChanged();
    void qSliderChanged();

private:
    Model(const Config& configuration, QObject *parent = nullptr);

    // This is the model-oriented filter struct. We use indexed values here.
    struct Filter {
        explicit Filter(common::FilterType t, uint8_t f, double g, uint8_t q);
        common::FilterType t;
        uint8_t f;
        double  g;
        uint8_t q;
    };

    void setFilters(const std::vector<Filter>& filters);

    void onParameterChanged();
    void onBleStatus(Status status, const QString& errorString);

    static Model* s_instance;

    const Config& m_configuration;

    Status          m_status = Status::Discovering;
    QString         m_statusReadout = "Discovering";
    QString         m_errorReadout;
    QList<Filter>   m_filters;
    Filter*         m_currentFilter = nullptr;
    int             m_currentBand = 0;
    double          m_freqSlider;
    bool            m_demoMode = false;

    // BLE
    ble::Central* m_central = nullptr;
    BleCentralAdapter* m_adapter = nullptr;
    friend class BleCentralAdapter;
};

#endif // MODEL_H
