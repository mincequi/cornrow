#ifndef MODEL_H
#define MODEL_H

#include <QObject>

#include "common/Types.h"

class Model : public QObject
{
    friend class MainWindow;

    Q_OBJECT

    Q_PROPERTY(int filterCount READ filterCount NOTIFY filterCountChanged)
    Q_PROPERTY(int currentFilter READ currentFilter WRITE setCurrentFilter NOTIFY currentFilterChanged)
    Q_PROPERTY(int type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString freqReadout READ freqReadout NOTIFY freqChanged)
    Q_PROPERTY(float gain READ gain WRITE setGain NOTIFY gainChanged)
    Q_PROPERTY(QString qReadout READ qReadout NOTIFY qChanged)
    Q_PROPERTY(float freqSlider READ freqSlider WRITE setFreqSlider NOTIFY freqSliderChanged)
    Q_PROPERTY(float qSlider READ qSlider WRITE setQSlider NOTIFY qSliderChanged)

public:
    Model(const std::vector<float>& freqTable,
          const std::vector<float>& qTable,
          QObject *parent = nullptr);
    Model(QObject *parent = nullptr);

    Q_INVOKABLE void addFilter();
    Q_INVOKABLE void deleteFilter();

    int         filterCount() const;

    int         currentFilter() const;
    Q_INVOKABLE void setCurrentFilter(int i);

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
    void currentFilterChanged();
    void typeChanged();
    void freqChanged();
    void freqSliderChanged();
    void gainChanged();
    void qChanged();
    void qSliderChanged();

private:
    struct Filter {
        FilterType    t = FilterType::Peak;
        int     f = 68;
        float   g = 0.0;
        int     q = 17;

        std::vector<float> mags;
        std::vector<float> phases;
    };

    const std::vector<float> m_freqTable;
    const std::vector<float> m_qTable;
    const int m_defaultFreq = 68;
    const int m_defaultQ = 17;
    const float m_minGain = -24.0;
    const float m_maxGain = 6.0;

    QList<Filter>   m_filters;
    Filter*         m_curFilter = nullptr;
    int             m_curIndex = -1;
    float           m_freqSlider;
};

#endif // MODEL_H
