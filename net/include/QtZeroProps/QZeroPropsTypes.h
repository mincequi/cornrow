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

#include <QMetaEnum>
#include <QUuid>

namespace QtZeroProps
{
Q_NAMESPACE

/// Configuration struct for QZeroPropsServer and QZeroPropsClient.
///
/// Provide a valid zeroConfType for using the WebSocket backend (e.g. _raop._tcp).
/// Provide a valid bleUuid for using the Bluetooth Low Energy backend.
struct Configuration
{
    QString zeroConfType;   ///< ZeroConf (WebSocket) service type to publish/discover
    QUuid   bleUuid;        ///< BluetoothLe service uuid to publish/discover
};

/// Supported device type / backend type.
enum class ServiceType : uint8_t {
    Invalid = 0,
    BluetoothLe = 0x1,
    WebSocket = 0x2,
    All = BluetoothLe | WebSocket
};
Q_ENUM_NS(ServiceType)

} // namespace QZeroProps
