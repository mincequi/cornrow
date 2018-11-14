#include "Config.h"

#include <common/Types.h>

Config* Config::s_instance = nullptr;

Config* Config::instance()
{
    return s_instance;
}

Config* Config::init(Type type)
{
    if (s_instance) {
        return s_instance;
    }

    s_instance = new Config(type);
    return s_instance;
}

Config::Config(Type type, QObject *parent)
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
        gainStep = 0.2;
        //qTable = common::qTableHigh; // @TODO(mawe): add 3 digit table
        qMax = qTable.size()-1;
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