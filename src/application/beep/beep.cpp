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
#include "lib/util/async/FunctionPointerRunnable.h"
#include "lib/util/ArgumentParser.h"
#include "lib/util/data/Array.h"
#include "lib/util/file/File.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/memory/String.h"
#include "lib/util/stream/FileOutputStream.h"
#include "lib/util/stream/InputStreamReader.h"
#include "lib/util/stream/PrintWriter.h"

static const constexpr uint8_t BAR_LENGTH = 25;

bool isRunning = true;

void printStatusLine(Util::File::File &speakerFile, uint32_t passedTime, uint32_t songLength) {
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

    auto line = reader.readLine();
    while (!line.isEmpty()) {
        length += Util::Memory::String::parseInt(line.split(",")[1]);
        line = reader.readLine();
    }

    return length;
}

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Play tunes from .beep-files via the PC speaker.\n"
                               "Usage: beep [FILE]\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "beep: No arguments provided!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto beepFile = Util::File::File(arguments[0]);
    if (!beepFile.exists() || beepFile.isDirectory()) {
        Util::System::error << "beep: '" << arguments[0] << "' could not be opened!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto speakerFile = Util::File::File("/device/speaker");
    auto fileReader = Util::Stream::FileReader(beepFile);
    auto reader = Util::Stream::BufferedReader(fileReader);
    auto outputStream = Util::Stream::FileOutputStream(speakerFile);
    auto writer = Util::Stream::PrintWriter(outputStream);

    uint32_t passedTime = 0;
    auto songLength = calculateLength(beepFile);

    Util::Async::Thread::createThread("Key-Listener", new Util::Async::FunctionPointerRunnable([]{
        Util::System::in.read();
        isRunning = false;
    }));

    auto line = reader.readLine();
    while (isRunning && !line.isEmpty()) {
        auto split = line.split(",");
        auto length = Util::Memory::String::parseInt(split[1]);
        passedTime += length;

        writer << split[0] << Util::Stream::PrintWriter::flush;
        printStatusLine(speakerFile, passedTime, songLength);

        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(length));
        Util::Graphic::Ansi::clearLineToCursor();
        Util::Graphic::Ansi::moveCursorToBeginningOfPreviousLine(0);
        line = reader.readLine();
    }

    writer << 0 << Util::Stream::PrintWriter::flush;
    Util::Graphic::Ansi::clearLine();
    return 0;
}