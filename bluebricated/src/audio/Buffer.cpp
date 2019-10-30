#include "Buffer.h"

#include <gstreamermm/allocator.h>
#include <gstreamermm/buffer.h>
#include <gstreamermm/memory.h>

Buffer::Buffer()
{
}

void* Buffer::obtain(int maxSize)
{
    m_memory = Gst::Allocator::get_default_allocator()->alloc(maxSize);
    m_memory->map(m_mapInfo, Gst::MAP_WRITE);
    return m_mapInfo.get_data();
}

void Buffer::commit(int size)
{
    m_memory->unmap(m_mapInfo);
    m_memory->resize(0, size);
}

Glib::RefPtr<Gst::Buffer> Buffer::buffer()
{
    auto buffer = Gst::Buffer::create();
    buffer->append_memory(std::move(m_memory));
    return buffer;
}
