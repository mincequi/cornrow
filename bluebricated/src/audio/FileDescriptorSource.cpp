#include "FileDescriptorSource.h"

#include "Pipeline.h"

#include <QDebug>
#include <QFile>
#include <QSocketNotifier>

FileDescriptorSource::FileDescriptorSource(int fd,
                                           uint16_t blockSize,
                                           audio::Pipeline* pipeline,
                                           QObject *parent) :
    QObject(parent),
    m_file(new QFile(this)),
    m_pipeline(pipeline),
    m_blockSize(blockSize)
{
    qDebug() << __func__ << "> fd:" << fd;
    if (!m_file->open(fd, QIODevice::ReadOnly, QFile::AutoCloseHandle)) {
        qWarning() << "error opening file descriptor:" << fd;
        return;
    }

    m_notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, [this](int fd) {
        static int blockSize = -1;
        auto buffer = m_pipeline->obtainBuffer(m_blockSize);
        auto size = m_file->read((char*)buffer, m_blockSize);
        m_pipeline->commitBuffer(size);
        if (blockSize != size) {
            qDebug() << "current block size:" << size;
            blockSize = size;
        }
    });
}
