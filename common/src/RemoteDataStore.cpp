#include "RemoteDataStore.h"

#include <common/IAudioConf.h>

#include <QMetaProperty>

namespace common {

RemoteDataStore::RemoteDataStore(IAudioConf* audio, QObject *parent)
    : QObject(parent),
      m_audio(audio)
{
    m_timer.setInterval(m_audio ? 0 : 200);
    m_timer.setSingleShot(true);

    connect(&m_timer, &QTimer::timeout, this, &RemoteDataStore::doNotify);
}

RemoteDataStore::~RemoteDataStore()
{
}

QByteArray RemoteDataStore::peq() const
{
    if (m_audio) {
        return m_converter.filtersToBle(m_audio->filters(common::ble::CharacteristicType::Peq));
    }

    return m_peq;
}

void RemoteDataStore::setPeq(const QByteArray& peq)
{
    m_peq = peq;
    if (m_audio) {
        m_audio->setFilters(common::ble::CharacteristicType::Peq, m_converter.filtersFromBle(peq));
    }
    setDirty("peq");
}

QByteArray RemoteDataStore::aux() const
{
    if (m_audio) {
        return m_converter.filtersToBle(m_audio->filters(common::ble::CharacteristicType::Aux));
    }

    return m_aux;
}

void RemoteDataStore::setAux(const QByteArray& aux)
{
    m_aux = aux;
    if (m_audio) {
        m_audio->setFilters(common::ble::CharacteristicType::Aux, m_converter.filtersFromBle(aux));
    }
    setDirty("aux");
}

QByteArray RemoteDataStore::ioCaps() const
{
    return m_ioCaps;
}

void RemoteDataStore::setIoConf(const QByteArray& ioConf)
{
    m_ioConf = ioConf;
    setDirty("ioConf");
}

void RemoteDataStore::setDirty(const std::string& property)
{
    m_dirtyProperties.insert(property);

    if (!m_timer.isActive()) {
        m_timer.start();
    }
}

void RemoteDataStore::doNotify()
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
