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
        qDebug() << "bytes available:" << m_file->bytesAvailable();

        // The allocFactor multiplies the space that is actually needed for a single block (since
        // there might be multiple blocks waiting on the file descriptor).
        //   * 4 seems to be enough for most cases.
        //   * 7 might be reasonable, so we can decode SBC frames in-place (but we need to allocate it for each slice).
        //   * 10 seems to be the max (for maxSize 672 and real size 608).
        int allocFactor = 4;
        auto buffer = new char[m_blockSize*allocFactor];
        auto size = m_file->read((char*)buffer, m_blockSize*allocFactor);
        // The number of slices to cut the buffer into.
        int slices = 1+(size/m_blockSize); // 608 -> 1, 1216 -> 2, 1824 -> 3, 2432 -> 4, 4864 -> 8
        if (size%slices != 0) {
            qWarning() << "cannot estimate number of slices";
            m_pipeline->pushBuffer(buffer, m_blockSize*allocFactor, size, 1);
        } else {
            m_pipeline->pushBuffer(buffer, m_blockSize*allocFactor, size, slices);
        }

        static int blockSize = -1;
        if (blockSize != size) {
            qDebug() << "current block size:" << size;
            blockSize = size;
        }
    });
}
