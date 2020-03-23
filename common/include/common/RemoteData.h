#pragma once

#include <set>

#include <QObject>
#include <QTimer>

#include <common/ble/Converter.h>

namespace common
{
class IAudioConf;

class RemoteData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray peq MEMBER m_peq NOTIFY peqChanged)
    Q_PROPERTY(QByteArray aux MEMBER m_aux NOTIFY auxChanged)
    Q_PROPERTY(QByteArray ioCaps READ ioCaps NOTIFY ioCapsChanged)
    Q_PROPERTY(QByteArray ioConf WRITE setIoConf)

public:
    RemoteData(IAudioConf* audio, QObject* parent = 0);
    ~RemoteData();

    QByteArray peq() const;
    void setPeq(const QByteArray& peq);

    QByteArray aux() const;
    void setAux(const QByteArray& aux);

    QByteArray ioCaps() const;
    void setIoConf(const QByteArray& ioConf);

signals:
    void peqChanged();
    void auxChanged();
    void ioCapsChanged();

private:
    void setDirty(const std::string& property);
    void doNotify();

    IAudioConf* m_audio;

    QTimer  m_timer;

    QByteArray m_peq;
    QByteArray m_aux;
    QByteArray m_ioCaps;
    QByteArray m_ioConf;

    std::set<std::string> m_dirtyProperties;

    common::ble::Converter m_converter;
};

} // namespace common
