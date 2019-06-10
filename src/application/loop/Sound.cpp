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

#include "kernel/core/Kernel.h"
#include "kernel/service/SoundService.h"
#include "Sound.h"

Sound::Sound() : Thread("Sound") {
    speaker = Kernel::getService<SoundService>()->getPcSpeaker();
    timeService = Kernel::getService<TimeService>();

    tetris = BeepFile::load("/initrd/music/beep/tetris.beep");
    mario = BeepFile::load("/initrd/music/beep/mario.beep");
}

void Sound::run () {
    while(true) {
        tetris->play();

        speaker->off();
        timeService->msleep(1000);

        mario->play();

        speaker->off();
        timeService->msleep(1000);
    }
}

Sound::~Sound() {
    speaker->off();

    delete tetris;
    delete mario;
}
