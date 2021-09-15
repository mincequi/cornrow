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

#include <coro/airplay/AirplaySource.h>
#include <coro/audio/AlsaSink.h>
#include <coro/audio/AudioConverter.h>
#include <coro/audio/AudioEncoderFfmpeg.h>
#include <coro/audio/Crossover.h>
#include <coro/audio/SbcDecoder.h>
#include <coro/audio/ScreamSource.h>
#include <coro/bluetooth/BluetoothSource.h>
#include <coro/core/AppSink.h>
#include <coro/core/AppSource.h>
#include <coro/core/FdSource.h>
#include <coro/core/SourceSelector.h>
#include <coro/core/TcpClientSink.h>
#include <coro/rtp/RtpDecoder.h>

#include <common/Types.h>

#include <optional>

namespace coro {
namespace audio {
class Loudness;
class Peq;
} // namespace audio
namespace pi {
class PiHdmiAudioSink;
} // namespace pi
} // namespace coro

class CoroPipeline {
public:
    struct Config {
        std::optional<coro::bluetooth::BluetoothSource::Config> bluetoothConfig;
        std::optional<coro::airplay::AirplaySource::Config> airplayConfig;
        std::optional<coro::core::TcpClientSink::Config> tcpConfig;
    };

    explicit CoroPipeline(const Config& config);
    ~CoroPipeline();

    void setFileDescriptor(int fd, uint16_t blockSize);

    void setVolume(float volume);
    void setLoudness(uint8_t phon);

    void setPeq(const std::vector<common::Filter>& filters);

    void setCrossover(const common::Filter& crossover);

    void setOutputDevice(const std::string& device);

    bool hasPiHdmiOutput() const;

    //void setRate(coro::audio::SampleRate rate);

    //common::Filter crossover() const;

private:
    // Airplay nodes
    std::optional<coro::airplay::AirplaySource> m_airplaySource;

    // Bluetooth nodes
    coro::core::FdSource    m_fdSource;
    coro::rtp::RtpDecoder<coro::audio::AudioCodec::Sbc> m_rtpDecoder;
    coro::audio::SbcDecoder m_sbcDecoder;

    // Scream node
    //coro::audio::ScreamSource m_screamSource;

    coro::audio::AudioConverter<int16_t, float> m_intToFloat;
    coro::audio::AudioConverter<float, int16_t> m_floatToInt;
    coro::audio::Peq*       m_peq       = nullptr;
    coro::audio::Loudness*  m_loudness  = nullptr;
    coro::audio::Crossover  m_crossover;
    coro::audio::AudioEncoderFfmpeg m_ac3Encoder;
    //coro::audio::AudioEncoderFfmpeg m_wavEncoder;
    coro::audio::AlsaSink   m_alsaSink;
    coro::core::TcpClientSink m_tcpSink;
    coro::pi::PiHdmiAudioSink*  m_piHdmiSink = nullptr;

    coro::core::SourceSelector m_sourceSelector;
};
