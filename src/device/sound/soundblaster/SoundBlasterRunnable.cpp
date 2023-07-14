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

#include "SoundBlasterRunnable.h"

#include "lib/util/async/Thread.h"
#include "device/sound/soundblaster/SoundBlaster.h"
#include "lib/util/base/Address.h"

namespace Device {

SoundBlasterRunnable::SoundBlasterRunnable(SoundBlaster &soundBlaster) : soundBlaster(soundBlaster) {
    inputStream->connect(*outputStream);
}

SoundBlasterRunnable::~SoundBlasterRunnable() {
    delete inputStream;
    delete outputStream;
}

void SoundBlasterRunnable::run() {
    while (isRunning) {
        inputStreamLock.acquire();
        uint32_t available = inputStream->available();
        inputStreamLock.release();

        while (available == 0) {
            if (isPlaying) {
                soundBlaster.waitForInterrupt();
                soundBlaster.turnSpeakerOff();
                isPlaying = false;
                dmaOffset = 0;
            }

            Util::Async::Thread::yield();

            inputStreamLock.acquire();
            available = inputStream->available();
            inputStreamLock.release();
        }

        const auto bufferSize = soundBlaster.getDmaBufferSize();
        auto *dmaBuffer = soundBlaster.getDmaBuffer();

        auto max = isPlaying ? bufferSize / 2 : bufferSize;
        if (available > max) {
            available = max;
        }

        if (isPlaying) {
            soundBlaster.waitForInterrupt();
        }

        inputStreamLock.acquire();
        inputStream->read(dmaBuffer, dmaOffset, available);
        inputStreamLock.release();

        if (available < bufferSize / 2) {
            Util::Address<uint32_t>(dmaBuffer + dmaOffset + available).setRange(0, bufferSize / 2 - available);
        }

        if (!isPlaying) {
            isPlaying = true;
            soundBlaster.turnSpeakerOn();
        }

        soundBlaster.play(dmaOffset, available);

        if (dmaOffset + available >= bufferSize) {
            dmaOffset = 0;
        } else {
            dmaOffset += available;
        }
    }
}

void SoundBlasterRunnable::stop() {
    isRunning = false;
}

void SoundBlasterRunnable::adjustInputStreamBuffer(uint16_t sampleRate, uint8_t channels, uint8_t bitsPerSample) {
    inputStreamLock.acquire();

    delete inputStream;
    inputStream = new Util::Io::PipedInputStream(static_cast<int32_t>(AUDIO_BUFFER_SIZE * sampleRate * (bitsPerSample / 8.0) * channels));
    inputStream->connect(*outputStream);

    inputStreamLock.release();
}

}