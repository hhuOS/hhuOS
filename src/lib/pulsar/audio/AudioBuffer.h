/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_LIB_PULSAR_AUDIOBUFFER_H
#define HHUOS_LIB_PULSAR_AUDIOBUFFER_H

#include "util/base/String.h"

namespace Pulsar {

/// Represents an audio buffer containing raw PCM samples loaded from a WAV file.
///
/// This class is used internally by the game engine and not intended for direct use by applications.
/// To load and play audio files, use the `AudioTrack` class instead.
class AudioBuffer {

public:
    /// Create an audio buffer instance by loading a WAV file from the specified path.
    /// The file is fully read into memory and the raw PCM samples are stored in this buffer.
    explicit AudioBuffer(const Util::String &waveFilePath);

    /// Audio buffer is not copyable, as it manages a dynamic memory buffer, so the copy constructor is deleted.
    AudioBuffer(const AudioBuffer &other) = delete;

    /// Audio buffer is not copyable, as it manages a dynamic memory buffer, so the assignment operator is deleted.
    AudioBuffer& operator=(const AudioBuffer &other) = delete;

    /// Get the path of the WAV file from which this audio buffer was loaded.
    [[nodiscard]] Util::String getWaveFilePath() const;

    /// Get the sample rate (samples per second) of the audio data in this buffer.
    [[nodiscard]] uint32_t getSamplesPerSecond() const;

    /// Get the bit depth (bits per sample) of the audio data in this buffer.
    [[nodiscard]] uint16_t getBitsPerSample() const;

    /// Get the number of audio channels (1 for mono, 2 for stereo, etc.) in this buffer.
    [[nodiscard]] uint16_t getNumChannels() const;

    /// Get the total size in bytes of the raw PCM audio data stored in this buffer.
    [[nodiscard]] uint32_t getSize() const;

    /// Get a pointer to the raw PCM audio samples stored in this buffer.
    [[nodiscard]] const uint8_t* getSamples() const;

private:

    Util::String waveFilePath = "";
    uint32_t samplesPerSecond = 0;
    uint16_t bitsPerSample = 0;
    uint16_t numChannels = 0;

    uint32_t size = 0;
    uint8_t *samples = nullptr;
};

}

#endif
