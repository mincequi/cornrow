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

#include <glibmm/refptr.h>

#include <common/Types.h>

#include <fstream>
#include <map>

namespace Gst
{
class Element;
class Pipeline;
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

    void start();
    void stop();

    Type type() const;

    void setVolume(float volume);

    void setCrossover(const common::Filter& crossover);
    common::Filter crossover() const;

    void setLoudness(uint8_t phon);

    void setOutputDevice(const std::string& device);

    void* obtainBuffer(int maxSize);
    void commitBuffer(int size);

private:
    bool constructPipeline(Type type, bool force = false);

    Type m_currentType = Type::Normal;

    Glib::RefPtr<Gst::Element>      m_crAppSource;
    Glib::RefPtr<Gst::Element>      m_alsaSink;
    Glib::RefPtr<Gst::Element>      m_alsaPassthroughSink;
    Glib::RefPtr<GstDsp::Crossover> m_crossover;
    Glib::RefPtr<GstDsp::Loudness>  m_loudness;
    Glib::RefPtr<GstDsp::Peq>       m_peq;
    Glib::RefPtr<Gst::Pipeline>     m_pipeline;
};

} // namespace audio
