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
#include "BeepFile.h"

BeepFile::BeepFile(File *file) {
    char *tmp;

    *file >> tmp;

    content = (Content*) tmp;

    speaker = Kernel::getService<SoundService>()->getPcSpeaker();
}

BeepFile::~BeepFile() {
    delete (char*) content;
}

BeepFile *BeepFile::load(const String &path) {
    BeepFile * ret = nullptr;

    File *file = File::open(path, "r");

    if(file == nullptr) {
        return nullptr;
    }

    uint32_t magic;

    file->readBytes(reinterpret_cast<char *>(&magic), 4);

    if(magic == MAGIC) {
        file->setPos(0, File::START);
        ret = new BeepFile(file);
    }

    delete file;

    return ret;
}

void BeepFile::play() {

    if (content->magic != MAGIC) {

        return;
    }

    isPlaying = true;

    for (uint32_t i = 0; isPlaying && i < content->length; i++) {

        speaker->play(content->sounds[i].frequency, content->sounds[i].length);
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
