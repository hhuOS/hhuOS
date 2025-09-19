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

#include "AudioRunnable.h"

#include "async/Thread.h"

namespace Util::Game {

AudioRunnable::AudioRunnable(Array<AudioChannel> &channels) : channels(channels) {}

void AudioRunnable::run() {
    isRunning = true;

    while (isRunning) {
        bool yield = true;

        for (auto &audioChannel : channels) {
            if (audioChannel.getState() == AudioChannel::PLAYING) {
                if (audioChannel.update()) {
                    // If at least one channel has written audio data, do not yield
                    yield = false;
                }
            }
        }

        if (yield) {
            // No channel has played any audio data -> No audio is currently played -> Yield the thread
            Async::Thread::yield();
        }
    }
}

void AudioRunnable::stop() {
    isRunning = false;
}

}