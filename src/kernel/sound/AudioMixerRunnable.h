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
 *
 * The audio mixer is based on a bachelor's thesis, written by Andreas Lüpertz.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-ANLU89
 */

#ifndef HHUOS_AUDIOMIXERRUNNABLE_H
#define HHUOS_AUDIOMIXERRUNNABLE_H

#include "lib/util/async/Runnable.h"
#include "lib/util/async/Spinlock.h"
#include "AudioMixer.h"

namespace Kernel {

class AudioMixer;

/**
 * Handles the transmission of data provided by @param &audioMixer
 */
class AudioMixerRunnable : public Util::Async::Runnable {

public:
    /**
     * Constructor.
     */
    explicit AudioMixerRunnable(AudioMixer &audioMixer, uint32_t bufferSize);

    /**
     * Copy Constructor.
     */
    AudioMixerRunnable(const AudioMixerRunnable &other) = delete;

    /**
     * Assignment operator.
     */
    AudioMixerRunnable &operator=(const AudioMixerRunnable &other) = delete;

    /**
     * Destructor.
     */
    ~AudioMixerRunnable() override;

    void run() override;

    void stop();

    void setMasterOutputDevice(Device::PcmDevice &device);

private:

    AudioMixer &audioMixer;
    Device::PcmDevice *masterOutputDevice = nullptr;

    bool isRunning = false;
    bool isStopped = false;
    uint32_t lastSampleCount = 0;

    uint32_t bufferSize;
    uint8_t *buffer = new uint8_t[bufferSize];
};

}

#endif
