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

#include "async/Runnable.h"
#include "collection/Array.h"
#include "game/audio/AudioChannel.h"

namespace Util::Game {

/// Runnable that manages a set of audio channels for playback.
/// The game engine creates a fixed pool of audio channels for playing sounds.
/// This runnable iterates over all channels and calls their `update()` method,
/// ensuring that they are continuously fed with audio data.
class AudioRunnable final : public Async::Runnable {

public:
    /// Create a new audio runnable that manages the specified array of audio channels.
    explicit AudioRunnable(Array<AudioChannel> &channels);

    /// Audio runnable is not copyable, so the copy constructor is deleted.
    AudioRunnable(const AudioRunnable &other) = delete;

    /// Audio runnable is not copyable, so the assignment operator is deleted.
    AudioRunnable& operator=(const AudioRunnable &other) = delete;

    /// Main loop of the runnable that continuously updates all audio channels.
    /// If all channels are idle or full, it yields the thread to save CPU resources.
    void run() override;

    /// Stop the runnable's main loop.
    /// This will cause the `run()` method to exit its loop and finish,
    /// stopping all audio channel updates for the game engine.
    void stop();

private:

    volatile bool isRunning = false;
    Array<AudioChannel> &channels;
};

}

#endif
