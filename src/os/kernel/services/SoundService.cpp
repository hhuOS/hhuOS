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

#include <devices/sound/SoundBlaster/SoundBlaster.h>
#include "SoundService.h"

Logger &SoundService::log = Logger::get("SOUND");

SoundService::SoundService() {
    log.info("Found audio device: PC Speaker");
    pcSpeaker = new PcSpeaker();
}

bool SoundService::isPcmAudioAvailable() {
    return pcmAudioDevice != nullptr;
}

PcSpeaker* SoundService::getPcSpeaker() {
    return pcSpeaker;
}

PcmAudioDevice *SoundService::getPcmAudioDevice() {
    return pcmAudioDevice;
}

void SoundService::setPcmAudioDevice(PcmAudioDevice *newDevice) {
    pcmAudioDevice = newDevice;

    if(pcmAudioDevice != nullptr) {
        log.info("PCM Audio Device is now set to '%s' by '%s'", (const char *) pcmAudioDevice->getDeviceName(),
                 (const char *) pcmAudioDevice->getVendorName());
    }
}
