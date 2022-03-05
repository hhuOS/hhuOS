/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <cstdint>
#include "lib/util/system/System.h"
#include "lib/util/stream/BufferedReader.h"
#include "lib/util/stream/InputStreamReader.h"
#include "lib/util/stream/FileInputStream.h"

Util::File::File getFile(const Util::Memory::String &path) {
    if (path[0] == '/') {
        return Util::File::File(path);
    }

    return Util::File::File(Util::File::getCurrentWorkingDirectory().getCanonicalPath() + "/" + path);
}

int32_t main(int32_t argc, char *argv[]) {
    if (argc < 2) {
        Util::System::out << "cat: No arguments provided!" << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;
        return -1;
    }

    for (int32_t i = 1; i < argc; i++) {
        Util::Memory::String path(argv[i]);
        auto file = getFile(path);
        if (!file.exists()) {
            Util::System::out << "cat: '" << path << "' not found!" << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;
            continue;
        }

        if (file.isDirectory()) {
            Util::System::out << "cat: '" << path << "' is a directory!" << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;
            continue;
        }

        auto fileType = file.getType();
        auto fileInputStream = Util::Stream::FileInputStream(file);
        auto fileReader = Util::Stream::InputStreamReader(fileInputStream);
        auto bufferedFileReader = Util::Stream::BufferedReader(fileReader);
        char logChar = bufferedFileReader.read();

        if (fileType == Util::File::REGULAR) {
            while (logChar != -1) {
                Util::System::out << logChar;
                logChar = bufferedFileReader.read();
            }
        } else {
            while (logChar != -1) {
                Util::System::out << logChar << Util::Stream::PrintWriter::flush;
                logChar = bufferedFileReader.read();
            }
        }
    }

    Util::System::out << Util::Stream::PrintWriter::flush;
    return 0;
}