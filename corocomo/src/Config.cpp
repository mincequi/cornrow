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
    peqFilterCount = 5;  // @TODO(mawe): implement dynamic filter count
    freqTable = common::frequencyTable;
    freqDefault = 144;
    freqMin = 8;
    freqMax = 248;
    freqStep = 8;
    gainMin = -12.0;
    gainMax = 3.0;
    gainStep = 1.0;
    qDefault = 34;
    qMin = 16;
    qMax = 64;
    qStep = 2;

    ioAvailable = true;
    loudnessAvailable = true;
    xoAvailable = true;
    swAvailable = true;

    switch (type) {
    case Type::Low:
        break;
    case Type::Mid:
        freqStep = 4;   // 2x
        gainMin = -24.0;
        gainMax = 6.0;
        gainStep = 0.5; // 2x
        qMin = 12;       // +14
        qMax = 80;
        break;
    case Type::High:
        freqStep = 2; // 8x
        gainMin = -48.0;
        gainMax = 12.0;
        gainStep = 0.5;
        qMin = 0;
        qMax = static_cast<uint8_t>(common::qTable.size()-1);
        qStep = 1;
        //swAvailable = true;
        break;
    }
}
