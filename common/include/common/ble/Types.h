/*
 * Copyright (C) 2020 Manuel Weichselbaumer <mincequi@web.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>
#include <string>

namespace common
{
namespace ble
{

enum class CharacteristicType : uint8_t {
    Invalid     = 0x00,

    Peq         = 0x01,
    Aux         = 0x02,
    IoCaps      = 0x04,
    IoConf      = 0x08
};

static const std::string cornrowServiceUuid      ("ad100000-d901-11e8-9f8b-f2801f1b9fd1");
static const std::string peqCharacteristicUuid   ("ad10e100-d901-11e8-9f8b-f2801f1b9fd1");
static const std::string auxCharacteristicUuid   ("ad10a100-d901-11e8-9f8b-f2801f1b9fd1");
static const std::string ioCapsCharacteristicUuid("ad101a00-d901-11e8-9f8b-f2801f1b9fd1");
static const std::string ioConfCharacteristicUuid("ad101f00-d901-11e8-9f8b-f2801f1b9fd1");
static const std::string presetCharacteristicUuid("ad105100-d901-11e8-9f8b-f2801f1b9fd1");

} // namespace ble
} // namespace common
