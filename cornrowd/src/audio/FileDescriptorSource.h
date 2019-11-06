#pragma once

#include <QObject>

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
                                  QObject *parent = nullptr);

signals:

public slots:

private:
    QFile* m_file;
    QSocketNotifier* m_notifier;

    audio::Pipeline* m_pipeline;
    uint16_t         m_blockSize = 0;
};
