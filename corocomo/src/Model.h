#ifndef MODEL_H
#define MODEL_H

#include <QObject>

#include <common/Types.h>

#include "ModelConfiguration.h"

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

    Q_PROPERTY(ModelConfiguration configuration MEMBER m_configuration CONSTANT)
    Q_PROPERTY(int filterCount READ filterCount NOTIFY filterCountChanged)
    Q_PROPERTY(int currentBand READ currentBand WRITE setCurrentBand NOTIFY currentBandChanged)

    Q_PROPERTY(uint type READ type WRITE setType NOTIFY typeChanged)

    Q_PROPERTY(float freqSlider READ freqSlider WRITE setFreqSlider NOTIFY freqSliderChanged)
    Q_PROPERTY(QString freqReadout READ freqReadout NOTIFY freqChanged)

    Q_PROPERTY(float gain READ gain WRITE setGain NOTIFY gainChanged)

    Q_PROPERTY(float qSlider READ qSlider WRITE setQSlider NOTIFY qSliderChanged)
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

    static Model* init(const ModelConfiguration& configuration);
    static Model* instance();

    Q_INVOKABLE void startDiscovering();
    Q_INVOKABLE void startDemoMode();

    Status      status() const;
    QString     statusReadout() const;
    QString     errorReadout() const;

    Q_INVOKABLE void resizeFilters(int diff);

    std::vector<qreal>  frequencyTable() const;
    int         filterCount() const;

    int         currentBand() const;
    Q_INVOKABLE void setCurrentBand(int i);

    int         type() const;
    void        setType(int type);

    QString     freqReadout() const;
    Q_INVOKABLE void stepFreq(int i);
    float       freqSlider() const;
    void        setFreqSlider(float);

    float       gain() const;
    Q_INVOKABLE void stepGain(int i);
    void        setGain(float g);

    QString     qReadout() const;
    Q_INVOKABLE void stepQ(int i);
    float       qSlider() const;
    void        setQSlider(float);

signals:
    void statusChanged();
    void filterCountChanged();
    void currentBandChanged();
    void filterChanged(int i, uchar t, float f, float g, float q);
    void typeChanged();
    void freqChanged();
    void freqSliderChanged();
    void gainChanged();
    void qChanged();
    void qSliderChanged();

private:
    Model(const ModelConfiguration& configuration, QObject *parent = nullptr);

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

    ModelConfiguration m_configuration;

    Status          m_status = Status::Discovering;
    QString         m_statusReadout = "Discovering";
    QString         m_errorReadout;
    QList<Filter>   m_filters;
    Filter*         m_currentFilter = nullptr;
    int             m_currentBand = 0;
    float           m_freqSlider;
    bool            m_demoMode = false;

    // BLE
    ble::Central* m_central = nullptr;
    BleCentralAdapter* m_adapter = nullptr;
    friend class BleCentralAdapter;
};

#endif // MODEL_H
