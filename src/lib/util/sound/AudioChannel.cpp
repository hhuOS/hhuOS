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
 *
 * The audio mixer is based on a bachelor's thesis, written by Andreas Lüpertz.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-ANLU89
 */

#include "AudioChannel.h"
#include "io/stream/FileInputStream.h"

namespace Util::Sound {

AudioChannel::AudioChannel() : audioMixerFile(AUDIO_MIXER_PATH), id(createChannel()),
        audioChannelFile(String::format("/device/channel%u", id)), outputStream(audioChannelFile) {}

AudioChannel::~AudioChannel() {
    audioMixerFile.controlFile(DELETE, Util::Array<size_t>({id}));
}

uint8_t AudioChannel::createChannel() {
    uint8_t id = 0;
    if (!audioMixerFile.controlFile(CREATE, Util::Array<size_t>({reinterpret_cast<size_t>(&id)}))) {
        Panic::fire(Panic::ILLEGAL_STATE, "No audio channel available!");
    }

    return id;
}

bool AudioChannel::stop(const bool flush) {
    // Flush any remaining audio data from the channel
    while (flush && getRemainingBytes() > 0) {
        Async::Thread::yield();
    }

    // Stop channel playback
    const auto success = audioMixerFile.controlFile(STOP, Util::Array<size_t>({id}));
    if (success) {
        playing = false;
    }

    return success;
}

bool AudioChannel::play() {
    const auto success = audioMixerFile.controlFile(PLAY, Util::Array<size_t>({id}));
    if (success) {
        playing = true;
    }

    return success;
}

bool AudioChannel::isPlaying() const {
    return playing;
}

size_t AudioChannel::getRemainingBytes() {
    size_t remainingBytes = 0;
    audioChannelFile.controlFile(GET_REMAINING_BYTES, {reinterpret_cast<size_t>(&remainingBytes)});

    return remainingBytes;
}

size_t AudioChannel::getWritableBytes() {
    size_t writableBytes = 0;
    audioChannelFile.controlFile(GET_WRITABLE_BYTES, {reinterpret_cast<size_t>(&writableBytes)});

    return writableBytes;
}

void AudioChannel::write(const uint8_t c) {
    outputStream.write(c);
}

void AudioChannel::write(const uint8_t *sourceBuffer, [[maybe_unused]] size_t offset, const size_t length) {
    outputStream.write(sourceBuffer, 0, length);
}

}