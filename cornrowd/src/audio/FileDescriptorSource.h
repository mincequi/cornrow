#pragma once

#include <QObject>

#include <coro/audio/AudioBuffer.h>

class CoroPipeline;
class QFile;
class QSocketNotifier;

namespace audio
{
class Pipeline;
}

class FileDescriptorSource : public QObject
{
    Q_OBJECT
public:
    explicit FileDescriptorSource(int fd,
                                  uint16_t blockSize,
                                  audio::Pipeline* pipeline,
                                  CoroPipeline* coroPipeline,
                                  QObject *parent = nullptr);

signals:

public slots:

private:
    void processOld();
    void processNew();

    QFile* m_file;
    QSocketNotifier* m_notifier;

    uint16_t         m_blockSize = 0;
    audio::Pipeline* m_pipeline;
    CoroPipeline*    m_coroPipeline;
    coro::audio::AudioBuffer m_coroBuffer;
};
