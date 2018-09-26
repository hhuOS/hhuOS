/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#ifndef HHUOS_PCMAUDIODEVICE_H
#define HHUOS_PCMAUDIODEVICE_H

#include <lib/String.h>
#include <lib/sound/Pcm.h>

class PcmAudioDevice {

protected:

    PcmAudioDevice() = default;

public:

    PcmAudioDevice(const PcmAudioDevice &copy) = delete;

    ~PcmAudioDevice() = default;

    virtual String getVendorName() = 0;

    virtual String getDeviceName() = 0;

    virtual void playPcmData(const Pcm &pcm) = 0;

    virtual void stopPlayback() = 0;
};

#endif
