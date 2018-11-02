#include "BleCentralAdapter.h"

#include <QTimer>

#include <ble/Central.h>

namespace ble
{

CentralAdapter::CentralAdapter(Central* central)
    : QObject(central),
      m_central(central)
{
    connect(m_central, &Central::characteristicRead, this, &CentralAdapter::onCharacteristicRead);

    QTimer::singleShot(4000, [this]() {
        emit peq( {
                      { common::FilterType::Peak, 60.0, -9.7, 25 },
                      { common::FilterType::LowPass, 111.0, 3.4, 11 },
                      { common::FilterType::HighPass, 11.0, -8.3, 14 }
                  });
    });
}

CentralAdapter::~CentralAdapter()
{
}

void CentralAdapter::onCharacteristicRead(common::FilterTask task, const QByteArray &value)
{

}

} // namespace ble
