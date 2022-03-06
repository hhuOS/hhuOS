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
#include "lib/util/graphic/Ansi.h"
#include "lib/util/system/System.h"

void lsDirectory(const Util::Memory::String &path) {
    const auto file = Util::File::getFile(path);
    if (!file.exists()) {
        Util::System::out << "ls: '" << path << "' not found!" << Util::Stream::PrintWriter::endl;
        return;
    }

    auto string = Util::Memory::String();
    if (file.isDirectory()) {
        for (const auto &child : file.getChildren()) {
            const auto currentFile = Util::File::File(file.getCanonicalPath() + "/" + child);
            string += Util::File::getFileColor(currentFile) + currentFile.getName() + (currentFile.isDirectory() ? "/" : "") + Util::Graphic::Ansi::RESET + " ";
        }

        string = string.substring(0, string.length() - 1);
    } else {
        string += Util::Graphic::Ansi::BRIGHT_YELLOW + file.getName() + Util::Graphic::Ansi::RESET;
    }

    Util::System::out << string << Util::Stream::PrintWriter::endl;
}

int32_t main(int32_t argc, char *argv[]) {
    if (argc <= 1) {
        lsDirectory(Util::File::getCurrentWorkingDirectory().getCanonicalPath());
    } else {
        if (argc == 2) {
            lsDirectory(argv[1]);
        } else {
            for (int32_t i = 1; i < argc; i++) {
                Util::System::out << argv[i] << ":" << Util::Stream::PrintWriter::endl;
                lsDirectory(argv[i]);
                if (i < argc - 1) {
                    Util::System::out << Util::Stream::PrintWriter::endl;
                }
            }
        }
    }

    Util::System::out << Util::Stream::PrintWriter::flush;
    return 0;
}