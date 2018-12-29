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

#include <kernel/services/SoundService.h>
#include <kernel/Kernel.h>
#include "SoundBlasterModule.h"
#include "SoundBlaster.h"

MODULE_PROVIDER {

    return new SoundBlasterModule();
};

int32_t SoundBlasterModule::initialize() {

    log = &Logger::get("SOUNDBLASTER");

    if(SoundBlaster::isAvailable()) {
        log->info("Found a SoundBlaster device");

        Kernel::getService<SoundService>()->setPcmAudioDevice(SoundBlaster::initialize());
    } else {
        log->info("No SoundBlaster device available");
    }

    return 0;
}

int32_t SoundBlasterModule::finalize() {
    return 0;
}

String SoundBlasterModule::getName() {
    return String();
}

Util::Array<String> SoundBlasterModule::getDependencies() {
    return Util::Array<String>(0);
}