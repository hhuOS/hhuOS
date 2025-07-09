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

#ifndef AUDIORUNNABLE_H
#define AUDIORUNNABLE_H

#include "async/Runnable.h"
#include "async/Spinlock.h"
#include "collection/Array.h"
#include "game/AudioChannel.h"

namespace Util::Game {

class AudioRunnable : public Async::Runnable {

public:
    /**
     * Constructor.
     */
    explicit AudioRunnable(Array<AudioChannel> &channels);

    /**
     * Copy Constructor.
     */
    AudioRunnable(const AudioRunnable &other) = delete;

    /**
     * Assignment operator.
     */
    AudioRunnable &operator=(const AudioRunnable &other) = delete;

    /**
     * Destructor.
     */
    ~AudioRunnable() override = default;

    void run() override;

    void stop();

private:

    bool isRunning = false;
    Array<AudioChannel> &channels;
};

}

#endif
