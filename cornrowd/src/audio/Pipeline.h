/*
 * Copyright (C) 2018 Manuel Weichselbaumer <mincequi@web.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <map>

#include <glibmm/refptr.h>

#include <common/Types.h>

namespace Gst
{
class Element;
class FdSrc;
class OutputSelector;
class Pad;
class Pipeline;
class Volume;
}

namespace coro
{
class Crossover;
class Loudness;
class Peq;
}

namespace audio
{

class Pipeline
{
public:
    enum class Type {
        Normal,
        Crossover
    };

    explicit Pipeline(Type type);
    ~Pipeline();

    void start();
    void stop();

    Type type() const;

    void setVolume(float volume);

    void setPeq(const std::vector<common::Filter>& filters);

    void setCrossover(const common::Filter& crossover);
    common::Filter crossover() const;

    void setLoudness(uint8_t phon);

    void setOutputDevice(const std::string& device);

    void pushBuffer(char* data, int maxSize, int size, int slices = 1);

    void setFileDescriptor(uint32_t sampleRate, int fd, uint32_t blockSize);

private:
    void constructPipeline(Type type);

    Type m_currentType = Type::Normal;

    // Common elements
    Glib::RefPtr<Gst::Pipeline>     m_pipeline;
    Glib::RefPtr<Gst::Element>      m_crAppSource;
    Glib::RefPtr<Gst::Element>      m_crFdSource;
    Glib::RefPtr<Gst::Element>      m_sbcDepay;
    Glib::RefPtr<Gst::Element>      m_sbcDecoder;
    Glib::RefPtr<Gst::Element>      m_bluetoothConverter;
    Glib::RefPtr<coro::Loudness>    m_loudness;
    Glib::RefPtr<coro::Peq>         m_peq;

    // Normal elements
    Glib::RefPtr<Gst::Element>      m_alsaConverter;
    Glib::RefPtr<Gst::Element>      m_alsaSink;

    // Crossover elements
    Glib::RefPtr<coro::Crossover>   m_crossover;
    Glib::RefPtr<Gst::Element>      m_ac3Encoder;
    Glib::RefPtr<Gst::Element>      m_alsaPassthroughSink;
};

} // namespace audio
