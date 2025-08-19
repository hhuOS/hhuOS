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

#ifndef KERNEL_AUDIOCHANNEL_H
#define KERNEL_AUDIOCHANNEL_H

#include "io/stream/Pipe.h"
#include "sound/AudioChannel.h"

namespace Kernel {

class AudioChannel : public Util::Io::Pipe {

public:
    /**
     * Default Constructor.
     */
    explicit AudioChannel(int32_t bufferSize);

    /**
     * Copy Constructor.
     */
    AudioChannel(const AudioChannel &other) = delete;

    /**
     * Assignment operator.
     */
    AudioChannel &operator=(const AudioChannel &other) = delete;

    /**
     * Destructor.
     */
    ~AudioChannel() override = default;

    bool write(uint8_t c) override;

    uint32_t write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

    void setState(Util::Sound::AudioChannel::State state);

    [[nodiscard]] Util::Sound::AudioChannel::State getState() const;

private:

    Util::Sound::AudioChannel::State state = Util::Sound::AudioChannel::STOPPED;

};

}

#endif
