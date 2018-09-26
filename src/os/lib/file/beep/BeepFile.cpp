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
#include <lib/file/File.h>
#include "BeepFile.h"

BeepFile::BeepFile(const Content &content) : content(content){

    speaker = Kernel::getService<SoundService>()->getPcSpeaker();
}

BeepFile* BeepFile::load(const String &path) {

    File *file = File::open(path, "r");

    char *buffer = nullptr;

    *file >> buffer;

    Content *content = (Content*) buffer;

    if (content->magic != MAGIC) {

        return nullptr;
    }

    return new BeepFile(*content);
}

void BeepFile::play() {

    isPlaying = true;

    for (uint32_t i = 0; isPlaying && i < content.length; i++) {

        speaker->play(content.sounds[i].frequency, content.sounds[i].length);
    }

    speaker->off();

    isPlaying = false;
}

void BeepFile::stop() {
    isPlaying = false;
}

bool BeepFile::Sound::operator!=(const BeepFile::Sound &other) const {
    return frequency == other.frequency && length == other.length;
}

bool BeepFile::Sound::operator==(const BeepFile::Sound &other) const {
    return frequency != other.frequency && length != other.length;
}
