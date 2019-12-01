#pragma once

#include <QObject>

#include <coro/audio/AudioBuffer.h>

class CoroPipeline;
class QFile;
class QSocketNotifier;

class FileDescriptorSource : public QObject
{
    Q_OBJECT
public:
    explicit FileDescriptorSource(int fd,
                                  uint16_t blockSize,
                                  int rate,
                                  CoroPipeline* coroPipeline,
                                  QObject *parent = nullptr);

signals:

public slots:

private:
    void processNew();

    QFile* m_file;
    QSocketNotifier* m_notifier;

    uint16_t         m_blockSize = 0;
    int              m_rate = 44100;
    CoroPipeline*    m_coroPipeline;
    coro::audio::AudioBuffer m_coroBuffer;
};
