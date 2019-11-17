#include "FileDescriptorSource.h"

#include "CoroPipeline.h"
#include "Pipeline.h"

#include <xxhash.h>

#include <QDebug>
#include <QFile>
#include <QSocketNotifier>
#include <QThread>

#include <coro/audio/AudioBuffer.h>
#include <coro/audio/AudioConf.h>
#include <loguru/loguru.hpp>

FileDescriptorSource::FileDescriptorSource(int fd,
                                           uint16_t blockSize,
                                           audio::Pipeline* pipeline,
                                           QObject *parent) :
    QObject(parent),
    m_file(new QFile(this)),
    m_blockSize(blockSize),
    m_pipeline(pipeline),
    m_coroPipeline(new CoroPipeline())
{
    qDebug() << __func__ << "> fd:" << fd;
    if (!m_file->open(fd, QIODevice::ReadOnly, QFile::AutoCloseHandle)) {
        qWarning() << "error opening file descriptor:" << fd;
        return;
    }

    m_notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, [this](int fd) {
        processNew();
    });
}

void FileDescriptorSource::processOld()
{
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
}

void FileDescriptorSource::processNew()
{
    // The allocFactor multiplies the space that is actually needed for a single block (since
    // there might be multiple blocks waiting on the file descriptor).
    //   * 4 seems to be enough for most cases.
    //   * 7 might be reasonable, so we can decode SBC frames in-place (but we need to allocate it for each slice).
    //   * 10 seems to be the max (for maxSize 672 and real size 608).

    int allocFactor = 10;
    coro::audio::AudioBuffer coroBuffer(m_blockSize*allocFactor);
    auto buffer = coroBuffer.acquire(m_blockSize*allocFactor);
    auto size = m_file->read((char*)buffer, m_blockSize*allocFactor);
    coroBuffer.commit(size);
    // The number of slices to cut the buffer into.
    int slices = 1+(size/m_blockSize); // 608 -> 1, 1216 -> 2, 1824 -> 3, 2432 -> 4, 4864 -> 8
    if (size%slices != 0) {
        LOG_F(WARNING, "cannot estimate number of slices");
        slices = 1;
    }

    coro::audio::AudioConf conf;
    conf.codec = coro::audio::Codec::Sbc;
    conf.isRtpPayloaded = true;

    if (slices > 1) {
        //LOG_F(INFO, "buffer hash before: %i", XXH32(coroBuffer.data(), coroBuffer.size(), 0));
        auto buffers = coroBuffer.split(size/slices, m_blockSize*allocFactor);
        //LOG_F(INFO, "buffer hash after: %i", XXH32(coroBuffer.data(), coroBuffer.size(), 0));
        for (auto& _buffer : buffers) {
            m_coroPipeline->pushBuffer(conf, _buffer);
        }
    } else {
        m_coroPipeline->pushBuffer(conf, coroBuffer);
    }

    static int blockSize = -1;
    if (blockSize != size) {
        LOG_F(INFO, "Current block size: %i", size);
        blockSize = size;
    }
}
