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
 */

#include "DesktopFile.h"

#include "util/io/file/File.h"
#include "util/io/stream/BufferedInputStream.h"
#include "util/io/stream/FileInputStream.h"

DesktopFile::DesktopFile(const Util::String &path) : DesktopFile(Util::Io::File(path)) {}

DesktopFile::DesktopFile(const Util::Io::File &file) {
    if (!file.exists() || file.isDirectory()) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Desktop entry file not found!");
    }

    Util::Io::FileInputStream inputStream(file);
    Util::Io::BufferedInputStream stream(inputStream);

    auto line = stream.readLine();
    do {
        const auto split = line.content.split("=", 2);
        if (split.length() == 2) {
            properties.put(split[0], split[1]);
        }

        line = stream.readLine();
    } while (!line.endOfFile);
}

Util::String DesktopFile::getProperty(const Util::String &key, const Util::String &defaultValue) const {
    if (properties.containsKey(key)) {
        return properties.get(key);
    }

    return defaultValue;
}
