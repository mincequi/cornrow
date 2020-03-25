#pragma once

#include <set>

#include <QObject>
#include <QTimer>

#include <common/ble/Converter.h>

namespace common
{
class IAudioConf;

class RemoteDataStore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray peq READ peq WRITE setPeq NOTIFY peqChanged)
    Q_PROPERTY(QByteArray aux READ aux WRITE setAux NOTIFY auxChanged)
    Q_PROPERTY(QByteArray ioCaps READ ioCaps NOTIFY ioCapsChanged)
    Q_PROPERTY(QByteArray ioConf MEMBER m_ioConf)

public:
    RemoteDataStore(IAudioConf* audio = nullptr, QObject* parent = nullptr);
    ~RemoteDataStore();

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

    IAudioConf* m_audio = nullptr;

    QTimer  m_timer;

    QByteArray m_peq;
    QByteArray m_aux;
    QByteArray m_ioCaps;
    QByteArray m_ioConf;

    std::set<std::string> m_dirtyProperties;

    common::ble::Converter m_converter;
};

} // namespace common
