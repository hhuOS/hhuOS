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

#include "SoundBlaster.h"

#include "lib/util/collection/Array.h"

namespace Util::Sound {

Sound::SoundBlaster::SoundBlaster(const Io::File &soundBlasterFile) : soundBlasterFile(soundBlasterFile), stream(soundBlasterFile) {}

bool SoundBlaster::setAudioParameters(uint32_t sampleRate, uint8_t channels, uint8_t bitsPerSample) {
    return soundBlasterFile.controlFile(Util::Sound::SoundBlaster::SET_AUDIO_PARAMETERS, Util::Array<uint32_t>({sampleRate, channels, bitsPerSample}));
}

void Sound::SoundBlaster::play(const uint8_t *data, uint32_t size) {
    stream.write(data, 0, size);
}

}