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

#ifndef SOUNDSERVICE_H
#define SOUNDSERVICE_H

#include "Service.h"
#include "device/sound/speaker/PcSpeaker.h"
#include "kernel/sound/AudioMixer.h"

namespace Kernel {

class SoundService : public Service {

public:
    /**
     * Constructor.
     */
    explicit SoundService() = default;

    /**
     * Copy Constructor.
     */
    SoundService(const SoundService &other) = delete;

    /**
     * Assignment operator.
     */
    SoundService &operator=(const SoundService &other) = delete;

    /**
     * Destructor.
     */
    ~SoundService() override = default;

    void setMasterOutputDevice(Device::PcmDevice &device) const;

    Device::PcSpeaker& getSpeaker();

    static const constexpr uint8_t SERVICE_ID = 11;

private:

    AudioMixer mixer;
    Device::PcSpeaker speaker;
};

}

#endif
