#ifndef MODEL_H
#define MODEL_H

#include <QObject>

#include "common/Types.h"

class Model : public QObject
{
    Q_OBJECT

    //Q_ENUMS(FilterType)

    Q_PROPERTY(int filterCount READ filterCount NOTIFY filterCountChanged)
    Q_PROPERTY(FilterType filterType READ filterType WRITE setFilterType NOTIFY filterTypeChanged)
    Q_PROPERTY(float freq READ freq NOTIFY freqChanged)
    Q_PROPERTY(float gain READ gain WRITE setGain NOTIFY gainChanged)
    Q_PROPERTY(float q READ q NOTIFY qChanged)
    Q_PROPERTY(float freqSlider READ freqSlider WRITE setFreqSlider NOTIFY freqSliderChanged)
    Q_PROPERTY(float qSlider READ qSlider WRITE setQSlider NOTIFY qSliderChanged)

public:
    explicit Model(const std::vector<float>& freqTable, QObject *parent = nullptr);

    Q_INVOKABLE void addFilter();
    Q_INVOKABLE void deleteFilter();
    Q_INVOKABLE void setFilter(int i);
    Q_INVOKABLE void stepFreq(int i);
    Q_INVOKABLE void stepQ(int i);

    int         filterCount() const;
    FilterType  filterType() const;
    void        setFilterType(FilterType type);
    float       freq() const;
    float       gain() const;
    void        setGain(float g);
    float       q() const;
    float       freqSlider() const;
    void        setFreqSlider(float);
    float       qSlider() const;
    void        setQSlider(float);

signals:
    void filterCountChanged();
    void filterTypeChanged();
    void freqChanged();
    void gainChanged();
    void qChanged();
    void freqSliderChanged();
    void qSliderChanged();

private:
    struct Filter {
        FilterType    t = FilterType::Peak;
        int     f = 68;
        float   g = 0.0;
        int     q = 17;
    };

    const std::vector<float> m_freqTable;
    QList<Filter>   m_filters;
    Filter*         m_curFilter = nullptr;
    int             m_curIndex = -1;
};

//Q_DECLARE_METATYPE(FilterType)

#endif // MODEL_H
