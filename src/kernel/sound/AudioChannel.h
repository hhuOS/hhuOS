/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

    void write(uint8_t c) override;

    void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

    void setState(Util::Sound::AudioChannel::State state);

    [[nodiscard]] Util::Sound::AudioChannel::State getState() const;

private:

    Util::Sound::AudioChannel::State state = Util::Sound::AudioChannel::STOPPED;

};

}

#endif
