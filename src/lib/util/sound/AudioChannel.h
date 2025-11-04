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
 * The audio mixer is based on a bachelor's thesis, written by Andreas Lüpertz.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-ANLU89
 */

#ifndef HHUOS_LIB_UTIL_SOUND_AUDIOCHANNEL_H
#define HHUOS_LIB_UTIL_SOUND_AUDIOCHANNEL_H

#include <stddef.h>

#include "util/io/file/File.h"
#include "util/io/stream/FileOutputStream.h"

namespace Util::Sound {

/// Creates and manages an audio channel for playback of audio data.
/// Audio samples can be written to the channel, which will then be played back through the audio mixer.
/// Currently, only 8-bit mono PCM audio samples at a sample rate of 22050 Hz are supported.
///
/// On instantiation, a new audio channel is created in the audio mixer.
/// Each channel has a unique ID and is can be accessed through a file in the `/device` directory.
/// This class handles the creation and deletion of audio channels, as well as playback control,
/// abstracting the underlying I/O operations and providing a simple interface for audio playback.
///
/// ### Example
/// ```c++
/// // Create a new audio channel (The constructor automatically requests a new audio channel from the audio mixer)
/// auto audioChannel = Util::Sound::AudioChannel();
///
/// // Open a wave file to play
/// auto waveFile = Util::Sound::WaveFile(Util::Io::File("audio.wav"));
///
/// // Create a buffer to hold the current chunk of audio data.
/// auto *buffer = new uint8_t[8192];
/// auto remaining = waveFile.getDataSize();
///
/// // Start playback of the audio channel (otherwise the mixer will ignore this channel)
/// audioChannel.play();
///
/// // Read the wave in small chunks and write them to the audio channel
/// while (remaining > 0) {
///     // Read a chunk of data from the wave file
///     const auto toRead = remaining >= 8192 ? 8192 : remaining;
///     waveFile.read(buffer, 0, toRead);
///
///     // Write the chunk to the audio channel for playback
///     audioChannel.write(buffer, 0, toRead);
///     remaining -= toRead;
/// }
///
/// // Stop playback of the audio channel (the channel is still valid, but the mixer will not play it for now)
/// audioChannel.stop();
/// ```
class AudioChannel : public Io::OutputStream {

public:
    /// Requests that can be issued to the audio mixer/channel via file control operations.
    enum Request {
        /// Create a new audio channel in the audio mixer.
        CREATE,
        /// Delete the audio channel from the audio mixer.
        DELETE,
        /// Start playback of the audio channel.
        PLAY,
        /// Stop playback of the audio channel.
        STOP,
        /// Get the current playback state of the audio channel (e.g. playing or stopped).
        GET_PLAYBACK_STATE,
        /// Get the amount of audio data currently in the channel.
        GET_REMAINING_BYTES,
        /// Get the amount of bytes, that can be written to the channel without blocking.
        GET_WRITABLE_BYTES
    };

    /// Possible states of the audio channel (e.g. playing or stopped).
    enum State {
        /// The audio channel is currently playing and audio data is being read from it by the audio mixer.
        /// Any data written to the channel will be played back as soon as possible.
        PLAYING,
        /// The audio channel has been stopped and the audio mixer is currently reading any remaining data.
        /// Writing to the channel will block until `play()` is called again.
        FLUSHING,
        /// The audio channel is stopped and the audio mixer is not reading any data from it.
        /// Writing to the channel will block until `play()` is called again.
        STOPPED,
    };

    /// Create a new audio channel instance.
    /// This will automatically request a new audio channel from the audio mixer.
    /// The mixer will assign a unique ID to the channel and create a corresponding file in the `/device` directory.
    /// This is file is later used to write audio data to the channel for playback.
    AudioChannel();

    /// Destroy this audio channel instance.
    /// This will automatically stop any ongoing playback and delete the audio channel from the audio mixer.
    /// The channel file in the `/device` directory will also be removed.
    ~AudioChannel() override;

    /// Start playback of the audio channel, by sending a request to the audio mixer.
    /// Before calling this, the mixer will not read any data from this channel.
    bool play();

    /// Stop playback by sending a request to the audio mixer.
    /// The mixer will then play any remaining data from the channel and stop playback afterward.
    /// When the channel is stopped, all write operations will block until `play()` is called again.
    bool stop();

    /// Get the current playback state of the audio channel.
    [[nodiscard]] State getState();

    /// Get the amount of audio data currently in the channel, waiting to be played.
    [[nodiscard]] size_t getRemainingBytes();

    /// Get the amount of bytes that can be written to the channel without blocking.
    /// It is always possible to write more bytes, but the write operation will block until the mixer reads some data.
    [[nodiscard]] size_t getWritableBytes();

    /// Write a single byte of audio data to the audio channel.
    /// The data must be in 8-bit mono PCM format at a sample rate of 22050 Hz.
    bool write(uint8_t c) override;

    /// Write a buffer of audio data to the audio channel.
    /// The data must be in 8-bit mono PCM format at a sample rate of 22050 Hz.
    uint32_t write(const uint8_t *sourceBuffer, size_t offset, size_t length) override;

private:

    uint8_t createChannel();

    bool playing = false;
    Io::File audioMixerFile;

    const uint8_t id;
    Io::File audioChannelFile;
    Io::FileOutputStream outputStream;

    static constexpr auto *AUDIO_MIXER_PATH = "/device/audiomixer";
};

}

#endif
