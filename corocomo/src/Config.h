#ifndef MODELCONFIGURATION_H
#define MODELCONFIGURATION_H

#include <QObject>
#include <QVector>

class Config : public QObject
{
    Q_OBJECT

    Q_PROPERTY(std::vector<double> freqTable MEMBER freqTable CONSTANT)
    Q_PROPERTY(double gainMin MEMBER gainMin CONSTANT)
    Q_PROPERTY(double gainMax MEMBER gainMax CONSTANT)
    Q_PROPERTY(double gainStep MEMBER gainStep CONSTANT)

public:
    enum class Type : uint8_t {
        Low,
        Mid,
        High
    };

    static Config* init(Type type);
    static Config* instance();

    uint8_t filterCount = 1;

    std::vector<double> freqTable;
    uint8_t freqDefault;
    uint8_t freqMin;
    uint8_t freqMax;
    uint8_t freqStep;

    std::vector<double> qTable;
    uint8_t qDefault;
    uint8_t qMin;
    uint8_t qMax;
    uint8_t qStep = 1;

    double gainMin = -24.0;
    double gainMax = 6.0;
    double gainStep = 1.0;

private:
    explicit Config(Type type, QObject *parent = nullptr);
    static Config* s_instance;
};
// QVector<qreal> is Qt 5.9 compatible. Qt 5.11 also accepts std::vector<double>.
//Q_DECLARE_METATYPE(std::vector<double>)

#endif // MODELCONFIGURATION_H
