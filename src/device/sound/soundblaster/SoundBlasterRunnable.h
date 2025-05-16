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

#ifndef HHUOS_SOUNDBLASTERRUNNABLE_H
#define HHUOS_SOUNDBLASTERRUNNABLE_H

#include <stdint.h>

#include "lib/util/async/Runnable.h"
#include "lib/util/io/stream/PipedOutputStream.h"
#include "lib/util/io/stream/PipedInputStream.h"
#include "lib/util/async/Spinlock.h"

namespace Device {
class SoundBlaster;

class SoundBlasterRunnable : public Util::Async::Runnable {

friend class SoundBlasterNode;

public:
    /**
     * Default Constructor.
     */
    explicit SoundBlasterRunnable(SoundBlaster &soundBlaster);

    /**
     * Copy Constructor.
     */
    SoundBlasterRunnable(const SoundBlasterRunnable &other) = delete;

    /**
     * Assignment operator.
     */
    SoundBlasterRunnable &operator=(const SoundBlasterRunnable &other) = delete;

    /**
     * Destructor.
     */
    ~SoundBlasterRunnable() override;

    void run() override;

    void stop();

    void adjustInputStreamBuffer(uint16_t sampleRate, uint8_t channels, uint8_t bitsPerSample);

private:

    bool isRunning = true;
    bool isPlaying = false;

    uint32_t dmaOffset = 0;
    SoundBlaster &soundBlaster;

    Util::Async::Spinlock inputStreamLock;

    Util::Io::PipedInputStream *inputStream = new Util::Io::PipedInputStream();
    Util::Io::PipedOutputStream *outputStream = new Util::Io::PipedOutputStream();

    static const constexpr double AUDIO_BUFFER_SIZE = 0.5;
};

}

#endif
