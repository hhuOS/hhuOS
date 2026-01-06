/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include "AudioMixerRunnable.h"

#include "util/async/Thread.h"
#include "util/time/Timestamp.h"

#include "AudioMixer.h"

namespace Kernel {

AudioMixerRunnable::AudioMixerRunnable(AudioMixer &audioMixer, uint32_t bufferSize) : audioMixer(audioMixer), bufferSize(bufferSize) {}

void AudioMixerRunnable::run() {
    isRunning = true;
    isStopped = false;

    while (isRunning) {
        // Read samples from the audio mixer
        // The audio mixer will mix the samples from all channels and return the number of processed bytes
        const auto toWrite = audioMixer.read(buffer, 0, bufferSize);
        if (toWrite == 0) {
            if (lastSampleCount != 0 && masterOutputDevice != nullptr) {
                // Signal to the master output device that the audio mixer is drained
                // This allows the device to handle the end of playback by e.g. turning off the speaker
                masterOutputDevice->sourceDrained();
            }

            // No samples to write, wait for more data
            Util::Async::Thread::yield();
            continue;
        }
        lastSampleCount = toWrite;

        // Calculate playback duration in milliseconds
        const auto durationMs = toWrite / (AudioMixer::BITS_PER_SAMPLE / 8) / AudioMixer::NUM_CHANNELS / (AudioMixer::SAMPLES_PER_SECOND / 1000);

        // Write the mixed audio samples to the master output stream
        if (masterOutputDevice == nullptr) {
            // Simulate playback by sleeping for the duration of the mixed audio samples
            Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(durationMs));
        } else {
            // Play the mixed audio samples on the master output device
            masterOutputDevice->play(buffer, toWrite);
            Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(durationMs / 2));
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