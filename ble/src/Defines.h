#pragma once

#include <QtBluetooth/QBluetoothUuid>

namespace common
{

static const QBluetoothUuid cornrowServiceUuid          (QUuid("ad100000-d901-11e8-9f8b-f2801f1b9fd1"));
static const QBluetoothUuid peqCharacteristicUuid       (QUuid("ad10e100-d901-11e8-9f8b-f2801f1b9fd1"));
static const QBluetoothUuid auxCharacteristicUuid       (QUuid("ad10a100-d901-11e8-9f8b-f2801f1b9fd1"));
static const QBluetoothUuid capsCharacteristicUuid      (QUuid("ad10c100-d901-11e8-9f8b-f2801f1b9fd1"));

} // namespace common
