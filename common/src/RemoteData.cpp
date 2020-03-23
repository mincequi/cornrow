#include "RemoteData.h"

#include <common/IAudioConf.h>

#include <QMetaProperty>

namespace common {

RemoteData::RemoteData(IAudioConf* audio, QObject *parent)
    : QObject(parent),
      m_audio(audio)
{
    m_timer.setInterval(200);
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, &RemoteData::doNotify);
}

RemoteData::~RemoteData()
{
}

QByteArray RemoteData::peq() const
{
    return m_converter.filtersToBle(m_audio->filters(common::ble::CharacteristicType::Peq));
}

void RemoteData::setPeq(const QByteArray& peq)
{
    m_peq = peq;
    setDirty("peq");
}

QByteArray RemoteData::aux() const
{
    return m_converter.filtersToBle(m_audio->filters(common::ble::CharacteristicType::Aux));
}

void RemoteData::setAux(const QByteArray& aux)
{
    m_aux = aux;
    setDirty("aux");
}

QByteArray RemoteData::ioCaps() const
{
    return m_ioCaps;
}

void RemoteData::setIoConf(const QByteArray& ioConf)
{
    m_ioConf = ioConf;
    setDirty("ioConf");
}

void RemoteData::setDirty(const std::string& property)
{
    m_dirtyProperties.insert(property);

    if (!m_timer.isActive()) {
        m_timer.start();
    }
}

void RemoteData::doNotify()
{
    for (int i = metaObject()->propertyOffset(); i < metaObject()->propertyCount(); ++i) {
        // If property is not dirty, continue
        if (!m_dirtyProperties.count(metaObject()->property(i).name())) {
            continue;
        }
        // If property has no notify signal, continue
        if (!metaObject()->property(i).hasNotifySignal()) {
            continue;
        }

        metaObject()->property(i).notifySignal().invoke(this);
    }

    m_dirtyProperties.clear();
}

} // namespace common
