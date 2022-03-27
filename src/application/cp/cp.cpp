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
#include "lib/util/stream/FileInputStream.h"

int32_t main(int32_t argc, char *argv[]) {
    if (argc < 3){
        Util::System::out << "cp: Missing arguments!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto sourceFile = Util::File::File(argv[1]);
    auto targetFile = Util::File::File(argv[2]);

    if (!sourceFile.exists()) {
        Util::System::out << "cp: '" << argv[1] << "' not found!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    if (!sourceFile.isFile()) {
        Util::System::out << "cp: '" << argv[1] << "' is a directory!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    if (targetFile.exists() && targetFile.isDirectory()) {
        targetFile = Util::File::File(targetFile.getCanonicalPath() + "/" + sourceFile.getName());
    }

    if (!targetFile.exists() && !targetFile.create(Util::File::REGULAR)) {
        Util::System::out << "cp: Failed to execute file '" << argv[2] << "'!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto source = Util::Stream::FileInputStream(sourceFile);
    auto target = Util::Stream::FileOutputStream(targetFile);

    auto *buffer = new uint8_t[4096];
    int32_t read;
    do {
        read = source.read(buffer, 0, 4096);
        if (read > 0) {
            target.write(buffer, 0, read);
        }
    } while (read != -1);

    delete[] buffer;
    return 0;
}