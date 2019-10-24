#pragma once

#include <cstdint>
#include <set>
#include <string>
#include <vector>

namespace common
{
namespace ble
{

enum class ServiceType : uint8_t {
    Invalid     = 0x00,

    CoroprotoV1 = 0x01,
    CoroprotoV2 = 0x02
};

enum class CharacteristicType : uint8_t {
    Invalid     = 0x00,

    Peq         = 0x01,
    Aux         = 0x02,
    IoCaps      = 0x04,
    IoConf      = 0x08
};

static const std::string cornrowServiceUuid    ("ad100000-d901-11e8-9f8b-f2801f1b9fd1");
static const std::string peqCharacteristicUuid ("ad10e100-d901-11e8-9f8b-f2801f1b9fd1");
static const std::string auxCharacteristicUuid ("ad10a100-d901-11e8-9f8b-f2801f1b9fd1");
static const std::string ioCapsCharacteristicUuid("ad101a00-d901-11e8-9f8b-f2801f1b9fd1");
static const std::string ioConfCharacteristicUuid("ad101f00-d901-11e8-9f8b-f2801f1b9fd1");

} // namespace ble
} // namespace common
