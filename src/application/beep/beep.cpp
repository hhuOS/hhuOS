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
#include "lib/util/stream/BufferedInputStream.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/async/Thread.h"
#include "lib/util/stream/FileReader.h"

int32_t main(int32_t argc, char *argv[]) {
    if (argc < 2) {
        Util::System::error << "music: No arguments provided!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto file = Util::File::File(argv[1]);
    if (!file.exists() || file.isDirectory()) {
        Util::System::error << "beep: '" << argv[1] << "' could not be opened!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto speakerFile = Util::File::File("/device/speaker");
    auto fileInputStream = Util::Stream::FileInputStream(file);
    auto inputStream = Util::Stream::BufferedInputStream(fileInputStream);
    auto outputStream = Util::Stream::FileOutputStream(speakerFile);
    auto writer = Util::Stream::PrintWriter(outputStream);

    Util::Memory::String currentBuffer;
    int16_t currentChar = fileInputStream.read();
    while(currentChar != -1) {
        if (currentChar == '\n') {
            auto split = currentBuffer.split(",");
            if (split.length() < 2) {
                Util::Exception::throwException(Util::Exception::Error::INVALID_ARGUMENT, "Invalid line in music file!");
            }

            writer << split[0] << Util::Stream::PrintWriter::flush;

            auto frequencyString = Util::Stream::FileReader(speakerFile).read(speakerFile.getLength()) + " Hz";
            Util::System::out << frequencyString  << Util::Stream::PrintWriter::flush;

            Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(Util::Memory::String::parseInt(split[1])));
            Util::Graphic::Ansi::clearLineToCursor();
            Util::Graphic::Ansi::setColumn(0);
            Util::System::out << "\u001b[1K\u001b[0G" << Util::Stream::PrintWriter::flush;

            currentBuffer = "";
        }

        currentBuffer += static_cast<char>(currentChar);
        currentChar = fileInputStream.read();
    }

    writer << 0 << Util::Stream::PrintWriter::flush;
    return 0;
}