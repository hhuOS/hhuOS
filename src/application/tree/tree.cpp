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

void treeDirectory(const Util::Memory::String &path, uint32_t level) {
    const auto file = Util::File::File(path);
    if (!file.exists()) {
        Util::System::out << "tree: '" << path << "' not found!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return;
    }

    auto string = Util::Memory::String("|-");
    for (uint32_t i = 0; i < level; i++) {
        string += "-";
    }

    string += Util::File::getFileColor(file) + file.getName() + (file.isDirectory() ? "/" : "") + Util::Graphic::Ansi::RESET + " ";
    Util::System::out << string << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;

    if (file.isDirectory()) {
        auto basePath = file.getCanonicalPath();
        for (const auto &child : file.getChildren()) {
            treeDirectory(basePath + "/" + child, level + 1);
        }
    }
}

int32_t main(int32_t argc, char *argv[]) {
    if (argc <= 1) {
        treeDirectory(Util::File::getCurrentWorkingDirectory().getCanonicalPath(), 0);
    } else {
        for (int32_t i = 1; i < argc; i++) {
            treeDirectory(argv[i], 0);
            if (i < argc - 1) {
                Util::System::out << Util::Stream::PrintWriter::endl;
            }
        }
    }

    Util::System::out << Util::Stream::PrintWriter::flush;
    return 0;
}