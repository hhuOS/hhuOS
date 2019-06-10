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

#ifndef __BeepFile_include__
#define __BeepFile_include__

#include "device/sound/PcSpeaker.h"
#include "lib/file/File.h"

class BeepFile {

    struct Sound {
        float frequency;
        uint32_t length;

        bool operator!=(const Sound &other) const;
        bool operator==(const Sound &other) const;
    };

    struct Content {
        uint32_t magic;
        uint32_t length;
        Sound sounds[];
    };

public:

    static BeepFile* load(const String &path);

    BeepFile(const BeepFile &other) = delete;

    BeepFile &operator=(const BeepFile &other) = delete;

    ~BeepFile();

    void play();

    void stop();

private:

    explicit BeepFile(File *file);

    const Content *content;

    PcSpeaker *speaker = nullptr;

    bool isPlaying = false;

    static constexpr const uint32_t MAGIC = 0x42454550;

};


#endif
