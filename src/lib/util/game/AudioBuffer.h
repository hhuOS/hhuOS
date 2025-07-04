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

#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include "base/String.h"

namespace Util::Game {

class AudioBuffer {

public:
    /**
     * Default Constructor.
     */
    AudioBuffer() = default;

    AudioBuffer(const String &waveFilePath);

    /**
     * Copy Constructor.
     */
    AudioBuffer(const AudioBuffer &other) = delete;

    /**
     * Assignment operator.
     */
    AudioBuffer &operator=(const AudioBuffer &other) = delete;

    /**
     * Destructor.
     */
    ~AudioBuffer() = default;

    [[nodiscard]] String getWaveFilePath() const;

    [[nodiscard]] uint32_t getSamplesPerSecond() const;

    [[nodiscard]] uint16_t getBitsPerSample() const;

    [[nodiscard]] uint16_t getNumChannels() const;

    [[nodiscard]] uint32_t getSize() const;

    [[nodiscard]] const uint8_t* getSamples() const;

private:

    String waveFilePath = "";
    uint32_t samplesPerSecond = 0;
    uint16_t bitsPerSample = 0;
    uint16_t numChannels = 0;

    uint32_t size = 0;
    uint8_t *samples = nullptr;
};

}

#endif
