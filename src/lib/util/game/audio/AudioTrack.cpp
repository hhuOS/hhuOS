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

#include "AudioTrack.h"

#include "game/Game.h"
#include "game/GameManager.h"
#include "game/ResourceManager.h"
#include "game/audio/AudioHandle.h"

namespace Util::Game {

Async::IdGenerator AudioTrack::idGenerator;

AudioTrack::AudioTrack(const String &waveFilePath) : id(idGenerator.getNextId() ?: idGenerator.getNextId()) {
    if (ResourceManager::hasAudioBuffer(waveFilePath)) {
        buffer = ResourceManager::getAudioBuffer(waveFilePath);
    } else {
        buffer = new AudioBuffer(waveFilePath);
        ResourceManager::addAudioBuffer(waveFilePath, buffer);
    }
}

AudioHandle AudioTrack::play(const bool loop) const {
    if (buffer == nullptr) {
        Panic::fire(Panic::INVALID_ARGUMENT, "AudioTrack: No audio buffer assigned!");
    }

    return GameManager::getGame().playAudioTrack(*this, loop);
}

const AudioBuffer& AudioTrack::getBuffer() const {
    return *buffer;
}

size_t AudioTrack::getId() const {
    return id;
}

}
