#ifndef BUFFER_H
#define BUFFER_H

#include <gstreamermm.h>

namespace Gst {
class Buffer;
}

class Buffer
{
public:
    Buffer();

    void* obtain(int maxSize);
    void commit(int size);

    Glib::RefPtr<Gst::Buffer>   buffer();

private:
    Glib::RefPtr<Gst::Memory>   m_memory;   // we need memory to have fine grained control (max size vs valid size).
    Glib::RefPtr<Gst::Buffer>   m_buffer;
    Gst::MapInfo    m_mapInfo;
};

#endif // BUFFER_H
