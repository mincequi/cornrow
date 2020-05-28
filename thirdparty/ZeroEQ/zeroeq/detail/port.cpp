
/* Copyright (c) 2015, Human Brain Project
 *                     Juan Hernando <jhernando@fi.upm.es>
 */

#include "port.h"

namespace zeroeq
{
namespace detail
{
uint16_t getPort(const std::string& name)
{
    const uint128_t& md5 = make_uint128(name);
    return 1024 + (md5.low() % (65535 - 1024));
}
}
}
