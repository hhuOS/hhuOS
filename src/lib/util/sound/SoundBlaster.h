/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_UTIL_SOUNDBLASTER_H
#define HHUOS_UTIL_SOUNDBLASTER_H

#include <cstdint>

#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileOutputStream.h"

namespace Util::Sound {

class SoundBlaster {

public:

    enum Request {
        SET_AUDIO_PARAMETERS,
    };

    /**
     * Constructor.
     */
    explicit SoundBlaster(const Io::File &soundBlasterFile);

    /**
     * Copy Constructor.
     */
    SoundBlaster(const SoundBlaster &other) = delete;

    /**
     * Assignment operator.
     */
    SoundBlaster &operator=(const SoundBlaster &other) = delete;

    /**
     * Destructor.
     */
    ~SoundBlaster() = default;

    bool setAudioParameters(uint32_t sampleRate, uint8_t channels, uint8_t bitsPerSample);

    void play(const uint8_t *data, uint32_t size);

private:

    Io::File soundBlasterFile;
    Io::FileOutputStream stream;
};

}

#endif
