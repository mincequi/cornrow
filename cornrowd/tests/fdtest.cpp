#include <assert.h>
#include <fstream>
#include <iostream>

#include <gstreamermm.h>
#include <gstreamermm-dsp.h>
#include <glibmm/main.h>

#include <QCoreApplication>
#include <QThread>

class Buffer
{
public:
    Buffer()
    {
    }

    void* obtain(int maxSize)
    {
        m_memory = Gst::Allocator::get_default_allocator()->alloc(maxSize);
        m_memory->map(m_mapInfo, Gst::MAP_WRITE);
        return m_mapInfo.get_data();
    }

    void commit(int size)
    {
        m_memory->unmap(m_mapInfo);
        m_memory->resize(0, size);
    }

    Glib::RefPtr<Gst::Buffer> buffer()
    {
        auto buffer = Gst::Buffer::create();
        buffer->append_memory(std::move(m_memory));
        return buffer;
    }

private:
    Glib::RefPtr<Gst::Memory>   m_memory;   // we need memory to have fine grained control (max size vs valid size).
    Glib::RefPtr<Gst::Buffer>   m_buffer;
    Gst::MapInfo    m_mapInfo;
}; // class Buffer

int main(int argc, char** argv)
{
    QCoreApplication a(argc, argv);

    assert(GstDsp::init());

    Buffer buffer;

    // Open output file
    std::fstream outFile(std::fstream("appsink.bin", std::ios::out | std::ios::binary));

    // Create pipeline
    auto pipeline = Gst::Pipeline::create();

    // Create source/sink
    auto src = Gst::AppSrc::create();
    auto sink = Gst::AppSink::create();

    // Attach signal handler to sink
    sink->property_emit_signals().set_value(true);
    sink->signal_new_sample().connect([&]() -> Gst::FlowReturn {
        auto sample = sink->pull_sample();
        Gst::MapInfo info;
        sample->get_buffer()->map(info, Gst::MAP_READ);
        outFile.write((char*)info.get_data(), info.get_size());
        sample->get_buffer()->unmap(info);
        return Gst::FLOW_OK;
    });

    // Link elements
    pipeline->add(src)->add(sink);
    src->link(sink);

    // Handle messages posted on bus
    pipeline->get_bus()->add_watch([&] (const Glib::RefPtr<Gst::Bus>&, const Glib::RefPtr<Gst::Message>& message) {
        switch (message->get_message_type()) {
        case Gst::MESSAGE_EOS:
        case Gst::MESSAGE_ERROR:
            pipeline->set_state(Gst::STATE_NULL);
            a.quit();
            break;
        default:
            break;
        }
        return true;
    });

    // Start pipline
    pipeline->set_state(Gst::STATE_PLAYING);

    // Start producer
    QThread* producer = QThread::create([&]() {
        for (int i = 0; i < 262144; ++i) {
            *((int*)buffer.obtain(4)) = i;
            buffer.commit(4);
            src->push_buffer(buffer.buffer());
        }
        src->end_of_stream();
    });
    producer->start();

    return a.exec();
}
