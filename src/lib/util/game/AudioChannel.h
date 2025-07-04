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

#ifndef AUDIOCHANNEL_H
#define AUDIOCHANNEL_H

#include "AudioBuffer.h"
#include "sound/AudioChannel.h"
#include "sound/WaveFile.h"

namespace Util::Game {

class AudioChannel : public Sound::AudioChannel {

public:
    /**
     * Default Constructor.
     */
    AudioChannel() = default;

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

    void play(const AudioBuffer &buffer, bool loop = false);

    void update(double delta);

    [[nodiscard]] String getWaveFilePath() const;

private:

    const AudioBuffer *buffer = nullptr;
    uint32_t position = 0;
    bool loop = false;
};

}

#endif
