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

#include "AudioMixerRunnable.h"
#include "AudioMixer.h"

namespace Kernel {

AudioMixerRunnable::AudioMixerRunnable(AudioMixer &audioMixer, uint32_t bufferSize) : audioMixer(audioMixer), bufferSize(bufferSize) {}

void AudioMixerRunnable::run() {
    while (isRunning) {
        // Read samples from the audio mixer
        // The audio mixer will mix the samples from all channels and return the number of processed bytes
        const auto toWrite = audioMixer.read(buffer, 0, bufferSize);

        // Write the mixed audio samples to the master output stream
        if (masterOutputDevice == nullptr) {
            Util::Async::Thread::yield();
        } else {
            if (toWrite > 0) {
                masterOutputDevice->play(buffer, toWrite);
            } else {
                masterOutputDevice->sourceDrained();
                Util::Async::Thread::yield();
            }
        }
    }

    isStopped = true;
}

void AudioMixerRunnable::stop() {
    isRunning = false;
    while (!isStopped) {
        Util::Async::Thread::yield();
    }
}

void AudioMixerRunnable::setMasterOutputDevice(Device::PcmDevice &device) {
    masterOutputDevice = &device;
}

AudioMixerRunnable::~AudioMixerRunnable() {
    delete[] buffer;
}

}