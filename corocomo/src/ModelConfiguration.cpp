#include "ModelConfiguration.h"

#include <common/Types.h>

ModelConfiguration* ModelConfiguration::s_instance = nullptr;

ModelConfiguration* ModelConfiguration::instance()
{
    return s_instance;
}

ModelConfiguration* ModelConfiguration::init(Type type)
{
    if (s_instance) {
        return s_instance;
    }

    s_instance = new ModelConfiguration(type);
    return s_instance;
}

ModelConfiguration::ModelConfiguration(Type type, QObject *parent)
    : QObject(parent)
{
    // lowConf
    filterCount = 5;  // @TODO(mawe): implement dynamic filter count
    freqTable = common::frequencyTable;
    freqDefault = 144;
    freqMin = 8;
    freqMax = 248;
    freqStep = 8;
    gainMin = -24.0;
    gainMax = 6.0;
    gainStep = 1.0;
    qTable = common::qTable;
    qDefault = 17;
    qMin = 14;
    qMax = 34;
    qStep = 1;

    switch (type) {
    case Type::Low:
        break;
    case Type::Mid:
        freqStep = 2;   // 4x
        gainStep = 0.5; // 2x
        qMin = 0;       // +14
        qMax = qTable.size()-1; // +17
        break;
    case Type::High:
        freqStep = 1; // 8x
        break;
    }

    freqTable.resize(0);
    for (auto it = common::frequencyTable.begin()+freqMin;
         it < common::frequencyTable.begin()+freqMax+1; it += freqStep) {
        freqTable.push_back(*it);
    }
    freqDefault = (freqDefault-freqMin)/freqStep;

    qTable.resize(0);
    for (auto it = common::qTable.begin()+qMin;
         it < common::qTable.begin()+qMax+1; it += qStep) {
        qTable.push_back(*it);
    }
    qDefault = (qDefault-qMin)/qStep;
}
