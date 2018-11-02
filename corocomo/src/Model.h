#ifndef MODEL_H
#define MODEL_H

#include <QObject>

#include <common/Types.h>

class Model : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int filterCount READ filterCount NOTIFY filterCountChanged)
    Q_PROPERTY(int currentBand READ currentBand WRITE setCurrentBand NOTIFY currentBandChanged)

    Q_PROPERTY(uint type READ type WRITE setType NOTIFY typeChanged)

    Q_PROPERTY(float freqSlider READ freqSlider WRITE setFreqSlider NOTIFY freqSliderChanged)
    Q_PROPERTY(QString freqReadout READ freqReadout NOTIFY freqChanged)

    Q_PROPERTY(float gain READ gain WRITE setGain NOTIFY gainChanged)

    Q_PROPERTY(float qSlider READ qSlider WRITE setQSlider NOTIFY qSliderChanged)
    Q_PROPERTY(QString qReadout READ qReadout NOTIFY qChanged)

public:
    static Model* instance();

    Q_INVOKABLE void addFilter();
    Q_INVOKABLE void deleteFilter();

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
    void        setGain(float g);

    QString     qReadout() const;
    Q_INVOKABLE void stepQ(int i);
    float       qSlider() const;
    void        setQSlider(float);

signals:
    void filterCountChanged();
    void filterAdded();
    void filterRemoved(int i);
    void currentBandChanged();
    void filterChanged(int i, uchar t, float f, float g, float q);
    void typeChanged();
    void freqChanged();
    void freqSliderChanged();
    void gainChanged();
    void qChanged();
    void qSliderChanged();

private:
    Model(const std::vector<float>& freqTable,
          const std::vector<float>& qTable,
          QObject *parent = nullptr);
    Model(QObject *parent = nullptr);

    void setFilters(const std::vector<common::Filter>& filters);

    void onParameterChanged();

    // This is the model-oriented filter struct. We use indexed values here.
    struct Filter {
        common::FilterType t = common::FilterType::Invalid;
        int     f = 68;
        float   g = 0.0;
        int     q = 17;
    };

    const std::vector<float> m_freqTable;
    const std::vector<float> m_qTable;
    const int m_defaultFreq = 68;
    const int m_minFreq = 0;
    const int m_maxFreq = 120;
    const int m_defaultQ = 17;
    const float m_minGain = -24.0;
    const float m_maxGain = 6.0;

    QList<Filter>   m_filters;
    Filter*         m_curFilter = nullptr;
    int             m_curIndex = 0;
    float           m_freqSlider;

    friend int main(int argc, char *argv[]);
};

#endif // MODEL_H
