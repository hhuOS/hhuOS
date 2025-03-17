/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <stdint.h>

#include "lib/util/base/System.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/BufferedInputStream.h"
#include "lib/util/io/stream/InputStream.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/sound/PcSpeaker.h"

static const constexpr uint8_t BAR_LENGTH = 25;

void printStatusLine(uint32_t passedTime, uint32_t songLength) {
    auto passedSeconds = passedTime / 1000;
    auto passedMinutes = passedSeconds / 60;
    auto totalSeconds = songLength / 1000;
    auto totalMinutes = totalSeconds / 60;

    auto timeString = Util::String::format("%u:%02um/%u:%02um", passedMinutes, passedSeconds - passedMinutes * 60, totalMinutes, totalSeconds - totalMinutes * 60);
    auto percentage = static_cast<double>(passedTime) / songLength;
    auto filledBar = static_cast<uint32_t>((BAR_LENGTH - 2) * percentage);

    Util::System::out << "[";
    for (uint32_t i = 0; i < filledBar; i++) {
        Util::System::out << "#";
    }
    for (uint32_t i = 0; i < (BAR_LENGTH - 2) - filledBar; i++) {
        Util::System::out << "-";
    }
    Util::System::out << "] " << timeString << Util::Io::PrintStream::flush;
}

uint32_t calculateLength(const Util::Io::File &beepFile) {
    auto fileStream = Util::Io::FileInputStream(beepFile);
    auto stream = Util::Io::BufferedInputStream(fileStream);
    bool endOfFile = false;
    uint32_t length = 0;

    auto line = stream.readLine(endOfFile);
    while (!endOfFile) {
        length += Util::String::parseInt(line.split(",")[1]);
        line = stream.readLine(endOfFile);
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
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "beep: No arguments provided!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto beepFile = Util::Io::File(arguments[0]);
    if (!beepFile.exists() || beepFile.isDirectory()) {
        Util::System::error << "beep: '" << arguments[0] << "' could not be opened!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto speaker = Util::Sound::PcSpeaker(Util::Io::File("/device/speaker"));
    auto fileStream = Util::Io::FileInputStream(beepFile);
    auto stream = Util::Io::BufferedInputStream(fileStream);

    uint32_t passedTime = 0;
    auto songLength = calculateLength(beepFile);

    Util::Graphic::Ansi::disableCursor();
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    Util::System::out << "Playing '" << beepFile.getName() << "'... Press <ENTER> to stop." << Util::Io::PrintStream::endl;

    bool endOfFile = false;
    auto line = stream.readLine(endOfFile);
    while (!endOfFile) {
        if (Util::System::in.read() > 0) {
            break;
        }

        auto split = line.split(",");
        auto frequency = Util::String::parseInt(split[0]);
        auto length = Util::String::parseInt(split[1]);

        Util::Graphic::Ansi::saveCursorPosition();
        printStatusLine(passedTime, songLength);
        Util::Graphic::Ansi::restoreCursorPosition();

        speaker.play(frequency, Util::Time::Timestamp::ofMilliseconds(length));
        passedTime += length;

        line = stream.readLine(endOfFile);
    }

    speaker.turnOff();

    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::moveCursorToBeginningOfPreviousLine(0);
    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::moveCursorToBeginningOfPreviousLine(0);
    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::enableCursor();

    return 0;
}