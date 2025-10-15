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

#ifndef HHUOS_LIB_UTIL_GAME_AUDIOBUFFER_H
#define HHUOS_LIB_UTIL_GAME_AUDIOBUFFER_H

#include <stddef.h>

#include "game/audio/AudioTrack.h"
#include "sound/AudioChannel.h"
#include "sound/WaveFile.h"

namespace Util::Game {

/// Represents an audio channel in the game engine that can play audio buffers.
/// It extends the functionality of the base `Sound::AudioChannel` class to support
/// playing `AudioBuffer` instances, which contain raw PCM audio samples loaded from WAV files.
///
/// The game engine manages a constant pool of audio channels for playback.
/// Each channel can play one audio buffer at a time, with support for looping.
///
/// This class is used internally by the game engine and not intended for direct use by applications.
/// To load and play audio files, use the `AudioTrack` class instead.
class AudioChannel final : public Sound::AudioChannel {

public:
    /// Create an audio channel instance with no audio buffer assigned.
    AudioChannel() = default;

    /// Audio channel is not copyable, as it manages playback state, so the copy constructor is deleted.
    AudioChannel(const AudioChannel &other) = delete;

    /// Audio channel is not copyable, as it manages playback state, so the assignment operator is deleted.
    AudioChannel& operator=(const AudioChannel &other) = delete;

    /// Play the specified audio buffer on this channel.
    /// If `loop` is true, the audio buffer will loop continuously until stopped.
    void play(const AudioTrack &buffer, bool loop = false);

    /// Write as many bytes as possible without blocking from the assigned audio buffer to the underlying channel.
    /// This method is called periodically by the game engine to keep the audio channel fed with data.
    /// Returns true if any data was written, false if no data could be written (i.e. channel is full).
    bool update();

    /// Get the ID of the currently playing audio track, or 0 if no track is assigned.
    [[nodiscard]] size_t getCurrentTrackId() const;

private:

    AudioTrack track;
    uint32_t position = 0;
    bool loop = false;
};

}

#endif
