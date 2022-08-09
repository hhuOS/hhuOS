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
#include "lib/util/time/Timestamp.h"
#include "lib/util/async/Thread.h"
#include "lib/util/stream/FileReader.h"
#include "lib/util/stream/BufferedReader.h"

static const constexpr uint8_t BAR_LENGTH = 25;

Util::Memory::String readLine(Util::Stream::BufferedReader &reader) {
    Util::Memory::String buffer;
    auto currentChar = reader.read();
    while (currentChar != -1 && currentChar != '\n') {
        buffer += currentChar;
        currentChar = reader.read();
    }

    return buffer;
}

void printStatusLine(const Util::File::File &speakerFile, uint32_t passedTime, uint32_t songLength) {
    auto frequencyString = Util::Stream::FileReader(speakerFile).read(speakerFile.getLength() - 1) + " Hz";
    auto percentage = static_cast<double>(passedTime) / songLength;
    auto filledBar = static_cast<uint32_t>((BAR_LENGTH - 2) * percentage);

    Util::System::out << "[";
    for (uint32_t i = 0; i < filledBar; i++) {
        Util::System::out << "#";
    }
    for (uint32_t i = 0; i < (BAR_LENGTH - 2) - filledBar; i++) {
        Util::System::out << "-";
    }
    Util::System::out << "]" << Util::Stream::PrintWriter::endl << frequencyString << Util::Stream::PrintWriter::flush;
}

uint32_t calculateLength(const Util::File::File &beepFile) {
    auto fileReader = Util::Stream::FileReader(beepFile);
    auto reader = Util::Stream::BufferedReader(fileReader);
    uint32_t length = 0;

    auto line = readLine(reader);
    while (!line.isEmpty()) {
        length += Util::Memory::String::parseInt(line.split(",")[1]);
        line = readLine(reader);
    }

    return length;
}

int32_t main(int32_t argc, char *argv[]) {
    if (argc < 2) {
        Util::System::error << "music: No arguments provided!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto beepFile = Util::File::File(argv[1]);
    if (!beepFile.exists() || beepFile.isDirectory()) {
        Util::System::error << "beep: '" << argv[1] << "' could not be opened!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto speakerFile = Util::File::File("/device/speaker");
    auto fileReader = Util::Stream::FileReader(beepFile);
    auto reader = Util::Stream::BufferedReader(fileReader);
    auto outputStream = Util::Stream::FileOutputStream(speakerFile);
    auto writer = Util::Stream::PrintWriter(outputStream);

    uint32_t passedTime = 0;
    auto songLength = calculateLength(beepFile);
    auto line = readLine(reader);

    while (!line.isEmpty()) {
        auto split = line.split(",");
        auto length = Util::Memory::String::parseInt(split[1]);
        passedTime += length;

        writer << split[0] << Util::Stream::PrintWriter::flush;
        printStatusLine(speakerFile, passedTime, songLength);

        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(length));
        Util::Graphic::Ansi::clearLineToCursor();
        Util::Graphic::Ansi::moveCursorToBeginningOfPreviousLine(0);
        line = readLine(reader);
    }

    writer << 0 << Util::Stream::PrintWriter::flush;
    Util::Graphic::Ansi::clearLine();
    return 0;
}