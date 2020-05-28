
/* Copyright (c) 2015, Human Brain Project
 *                     Stefan.Eilemann@epfl.ch
 *                     Juan Hernando <jhernando@fi.upm.es>
 */

#ifndef ZEROEQ_DETAIL_PORT_H
#define ZEROEQ_DETAIL_PORT_H

#include <zeroeq/api.h>
#include <zeroeq/types.h>

namespace zeroeq
{
namespace detail
{
/** @return a fixed, pseudo-random port for the given name. */
uint16_t getPort(const std::string& name);
}
}

#endif
