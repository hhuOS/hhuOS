/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LIB_PULSAR_AUDIORUNNABLE_H
#define HHUOS_LIB_PULSAR_AUDIORUNNABLE_H

#include "util/async/Runnable.h"
#include "util/collection/Array.h"
#include "pulsar/audio/AudioChannel.h"

namespace Pulsar {

/// Runnable that manages a set of audio channels for playback.
/// The game engine creates a fixed pool of audio channels for playing sounds.
/// This runnable iterates over all channels and calls their `update()` method,
/// ensuring that they are continuously fed with audio data.
class AudioRunnable final : public Util::Async::Runnable {

public:
    /// Create a new audio runnable that manages the specified array of audio channels.
    explicit AudioRunnable(Util::Array<AudioChannel> &channels) : channels(channels) {}

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
    void stop() {
        isRunning = false;
    }

private:

    volatile bool isRunning = false;
    Util::Array<AudioChannel> &channels;
};

}

#endif
