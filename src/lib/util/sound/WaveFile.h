/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HHUOS_WAVEFILE_H
#define HHUOS_WAVEFILE_H

#include <cstdint>

#include "lib/util/io/stream/FilterInputStream.h"
#include "lib/util/io/stream/FileInputStream.h"

namespace Util {
namespace Io {
class File;
}  // namespace Io
}  // namespace Util

namespace Util::Sound {

class WaveFile : public Io::FilterInputStream {

public:

    enum AudioFormat : uint16_t {
        PCM = 0x0001,
        MS_ADPCM = 0x0002,
        IEEE_FLOAT = 0x0003,
        IBM_CVSD = 0x0005,
        ALAW = 0x0006,
        MULAW = 0x0007,
        OKI_ADPCM = 0x0010,
        DVI_IMA_ADPCM = 0x0011,
        MEDIASPACE_ADPCM = 0x0012,
        SIERRA_ADPCM = 0x0013,
        G723_ADPCM = 0x0014,
        DIGISTD = 0x0015,
        DIGIFX = 0x0016,
        DIALOGIC_OKI_ADPCM = 0x0017,
        YAMAHA_ADPCM = 0x0020,
        SONARC = 0x0021,
        DSPGROUP_TRUESPEECH = 0x0022,
        ECHOSC1 = 0x0023,
        AUDIOFILE_AF36 = 0x0024,
        APTX = 0x0025,
        AUDIOFILE_AF10 = 0x0026,
        DOLBY_AC2 = 0x0030,
        GSM610 = 0x0031,
        ANTEX_ADPCME = 0x0032,
        CONTROL_RES_VQLPC_1 = 0x0034,
        CONTROL_RES_VQLPC_2 = 0x0035,
        DIGIADPCM = 0x0036,
        CONTROL_RES_CR10 = 0x0037,
        NMS_VBXADPCM = 0x0038,
        CS_IMAADPCM = 0x0039,
        G721_ADPCM = 0x0040,
        MPEG_1_2 = 0x0050,
        MPEG_3 = 0x0055,
        XBOX_ADPCM = 0x0069,
        CREATIVE_ADPCM = 0x0200,
        CREATIVE_FASTSPEECH8 = 0x0202,
        CREATIVE_FASTSPEECH10 = 0x0203,
        FM_TOWNS_SND = 0x0300,
        OLIGSM = 0x1000,
        OLIADPCM = 0x1001,
        OLICELP = 0x1002,
        OLISBC = 0x1003,
        OLIOPR = 0x1004
    };

    /**
     * Constructor.
     */
    explicit WaveFile(const Io::File &file);

    /**
     * Copy Constructor.
     */
    WaveFile(const WaveFile &other) = delete;

    /**
     * Assignment operator.
     */
    WaveFile &operator=(const WaveFile &other) = delete;

    /**
     * Destructor.
     */
    ~WaveFile() override = default;

    [[nodiscard]] AudioFormat getAudioFormat() const;

    [[nodiscard]] uint16_t getNumChannels() const;

    [[nodiscard]] uint32_t getSamplesPerSecond() const;

    [[nodiscard]] uint32_t getBytesPerSecond() const;

    [[nodiscard]] uint16_t getBitsPerSample() const;

    [[nodiscard]] uint16_t getFrameSize() const;

    [[nodiscard]] uint32_t getSampleCount() const;

    [[nodiscard]] uint32_t getDataSize() const;

private:

    struct FormatChunk {
        char formatSignature[4];
        uint32_t chunkSize;
        AudioFormat audioFormat;
        uint16_t numChannels;
        uint32_t samplesPerSecond;
        uint32_t bytesPerSecond;
        uint16_t frameSize;
        uint16_t bitsPerSample;
    } __attribute__ ((packed));

    struct DataChunk {
        char dataSignature[4];
        uint32_t chunkSize;
    } __attribute__ ((packed));

    struct RiffChunk {
        char riffSignature[4];
        uint32_t chunkSize;
        char waveSignature[4];
    } __attribute__ ((packed));

    Io::FileInputStream stream;

    RiffChunk riffChunk{};
    FormatChunk formatChunk{};
    DataChunk dataChunk{};
};

}

#endif
