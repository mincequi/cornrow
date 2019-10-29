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

namespace GstDsp
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

    Type type() const;

    void setTransport(int fd, uint blocksize, int rate);

    void setVolume(float volume);

    void setCrossover(const common::Filter& crossover);
    common::Filter crossover() const;

    void setLoudness(uint8_t phon);

    void setOutputDevice(const std::string& device);

private:
    bool constructPipeline(Type type, bool force = false);

    Type m_currentType = Type::Normal;

    Glib::RefPtr<Gst::Element>      m_bluetoothSource;
    //Glib::RefPtr<Gst::FdSrc>        m_bluetoothSource;
    Glib::RefPtr<Gst::Element>      m_alsaSink;
    Glib::RefPtr<Gst::Element>      m_alsaPassthroughSink;
    Glib::RefPtr<GstDsp::Crossover> m_crossover;
    Glib::RefPtr<GstDsp::Loudness>  m_loudness;
    Glib::RefPtr<GstDsp::Peq>       m_peq;
    Glib::RefPtr<Gst::Pipeline>     m_pipeline;

    std::map<Type, std::vector<Glib::RefPtr<Gst::Element>>> m_elements;
};

} // namespace audio
