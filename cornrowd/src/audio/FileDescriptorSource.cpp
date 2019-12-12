#include "FileDescriptorSource.h"

#include "CoroPipeline.h"
#include "Pipeline.h"

#include <QFile>
#include <QSocketNotifier>
#include <QThread>

#include <coro/audio/AudioBuffer.h>
#include <coro/audio/AudioConf.h>
#include <loguru/loguru.hpp>

FileDescriptorSource::FileDescriptorSource(int fd,
                                           uint16_t blockSize,
                                           int rate,
                                           CoroPipeline* coroPipeline,
                                           QObject *parent) :
    QObject(parent),
    m_file(new QFile(this)),
    m_blockSize(blockSize),
    m_rate(rate),
    m_coroPipeline(coroPipeline),
    m_coroBuffer(m_blockSize*10)
{
    LOG_F(INFO, "opening file descriptor: %d", fd);
    if (!m_file->open(fd, QIODevice::ReadOnly, QFile::AutoCloseHandle)) {
        LOG_F(ERROR, "error opening file descriptor: %d", fd);
        return;
    }

    m_notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, [this](int fd) {
        processNew();
    });
}

void FileDescriptorSource::processNew()
{
    // The allocFactor multiplies the space that is actually needed for a single block (since
    // there might be multiple blocks waiting on the file descriptor).
    //   * 4 seems to be enough for most cases.
    //   * 7 might be reasonable, so we can decode SBC frames in-place (but we need to allocate it for each slice).
    //   * 10 seems to be the max (for maxSize 672 and real size 608).

    int allocFactor = 10;
    auto buffer = m_coroBuffer.acquire(m_blockSize*allocFactor);

    // Read all data from filedescriptor. We have to cut the data to frames, otherwise, the decoder will get in trouble.
    auto size = m_file->read((char*)buffer, m_blockSize*allocFactor);
    m_coroBuffer.commit(size);
    // The number of slices to cut the buffer into.
    int slices = 1+(size/m_blockSize); // 608 -> 1, 1216 -> 2, 1824 -> 3, 2432 -> 4, 4864 -> 8
    if (size%slices != 0) {
        LOG_F(WARNING, "cannot estimate number of slices");
        slices = 1;
    }

    coro::audio::AudioConf conf;
    conf.codec = coro::audio::AudioCodec::Sbc;
    conf.rate = m_rate == 48000 ? coro::audio::SampleRate::Rate48000 : coro::audio::SampleRate::Rate44100;
    conf.isRtpPayloaded = true;

    if (slices > 1) {
        // @TODO(mawe): we split buffers here, but allocate more memory to avoid reallocation.
        //              actually, original buffer is smaller, but will be reused, so reallocation
        //              takes place only once.
        auto buffers = m_coroBuffer.split(size/slices, m_blockSize*allocFactor*2);
        //LOG_F(INFO, "buffer hash after: %i", XXH32(coroBuffer.data(), coroBuffer.size(), 0));
        for (auto& _buffer : buffers) {
            m_coroPipeline->pushBuffer(conf, _buffer);
        }
    } else {
        m_coroPipeline->pushBuffer(conf, m_coroBuffer);
    }

    static int blockSize = -1;
    if (blockSize != size) {
        LOG_F(2, "Current block size: %i", (int)size);
        blockSize = size;
    }
}
