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

#ifndef HHUOS_LIB_UTIL_GAME_AUDIOHANDLE_H
#define HHUOS_LIB_UTIL_GAME_AUDIOHANDLE_H

#include <stddef.h>

#include "game/audio/AudioChannel.h"

namespace Util::Game {

/// Handle to control playback of an audio track.
/// An `AudioHandle` is returned when starting playback of an `AudioTrack`.
/// It can be used to check if the audio is still playing and to stop playback.
/// See `AudioTrack` for a code example.
class AudioHandle {

public:
    /// Create an empty audio handle instance that does not control any playback.
    /// Usable instances must be obtained from `AudioTrack::play()`.
    AudioHandle() = default;

    /// Check if the audio track associated with this handle is currently playing.
    /// If the audio has finished playing or was stopped, this will return false.
    /// If the handle is empty (i.e. not associated with any playback), this will also return false.
    [[nodiscard]] bool isPlaying() const;

    /// Stop playback of the audio track associated with this handle.
    /// If the audio is not currently playing or the handle is empty, this does nothing.
    void stop() const;

private:

    friend class Game;

    explicit AudioHandle(AudioChannel &channel, const AudioTrack &track);

    AudioChannel *channel = nullptr;
    size_t trackId = 0;
};

}

#endif
