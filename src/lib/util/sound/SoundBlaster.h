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
 */

#ifndef HHUOS_LIB_UTIL_SOUND_SOUNDBLASTER_H
#define HHUOS_LIB_UTIL_SOUND_SOUNDBLASTER_H

#include <stddef.h>
#include <stdint.h>

#include "io/file/File.h"
#include "io/stream/FileOutputStream.h"

namespace Util::Sound {

/// This class provides a user space interface to the Sound Blaster audio device.
/// The kernel driver for the Sound Blaster exposes a file interface, which allows audio samples to be written to it,
/// that are then played by the Sound Blaster hardware.
/// Audio parameters (sample rate, number of channels and bits per sample) are set using a control file operation.
/// This class wraps that file interface and exposes a more convenient API for playing audio samples.
/// The driver supports Sound Blaster 1, Sound Blaster 2, Sound Blaster Pro and Sound Blaster 16 compatible devices
/// and up to 16-bit stereo audio samples at 44100 Hz. However, during testing with QEMU we found problems with
/// 16-bit stereo audio playback, so we recommend using 8-bit mono audio samples for maximum compatibility.
///
/// ### Example
/// ```c++
/// auto sb = Util::Sound::SoundBlaster(soundBlasterFile); // Create a new SoundBlaster instance
/// auto waveFile = Util::Sound::WaveFile("audio.wav"); // Open a wave file to play
///
/// // Set the audio parameters for the Sound Blaster device based on the wave file.
/// // (This examples assumes the wave file is valid and does not check the return value)
/// sb.setAudioParameters(waveFile.getSamplesPerSecond(), waveFile.getNumChannels(), waveFile.getBitsPerSample());
///
/// // Read the wave file data and play it through the Sound Blaster.
/// auto *buffer = new uint8_t[waveFile.getDataSize()]; // Allocate a buffer for audio data
/// waveFile.read(buffer, 0, waveFile.getDataSize()); // Read the entire wave file into the buffer
/// sb.play(buffer, waveFile.getDataSize()); // Play the audio data through the Sound Blaster
/// ```

class SoundBlaster {

public:
    /// Control file operations for the Sound Blaster device.
    enum Request {
        SET_AUDIO_PARAMETERS,
    };

    /// Create a new SoundBlaster instance using the specified file.
    /// The file should point to the Sound Blaster device file, typically `/device/soundblaster`.
    explicit SoundBlaster(const Io::File &soundBlasterFile);

    /// SoundBlaster is not copyable, since it contains a not copyable `FileOutputStream`,
    /// so the copy constructor is deleted.
    SoundBlaster(const SoundBlaster &other) = delete;

    /// SoundBlaster is not assignable, since it contains a not copyable `FileOutputStream`,
    /// so the assignment operator is deleted.
    SoundBlaster& operator=(const SoundBlaster &other) = delete;

    /// The SoundBlaster destructor is trivial, as it only contains a `File` and a `FileOutputStream`.
    ~SoundBlaster() = default;

    /// Set the audio parameters for the Sound Blaster device.
    /// Not setting or using wrong audio parameters will likely result in playback issues,
    /// such as no sound, distorted audio or wrong playback speed.
    bool setAudioParameters(uint16_t sampleRate, uint8_t channels, uint8_t bitsPerSample);

    /// Play the given audio data through the Sound Blaster device.
    /// The data should be in the format specified by the audio parameters set with `setAudioParameters()`
    /// and only contains raw audio samples without any headers.
    void play(const uint8_t *data, size_t size);

private:

    Io::File soundBlasterFile;
    Io::FileOutputStream stream;
};

}

#endif
