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

#include <stdint.h>

#include <util/base/System.h>
#include <util/base/String.h>
#include <util/base/ArgumentParser.h>
#include <util/collection/Array.h>
#include <util/graphic/Ansi.h>
#include <util/time/Timestamp.h>
#include <util/sound/PcSpeaker.h>
#include <util/io/file/File.h>
#include <util/io/key/KeyDecoder.h>
#include <util/io/key/layout/DeLayout.h>
#include <util/io/stream/FileInputStream.h>
#include <util/io/stream/BufferedInputStream.h>
#include <util/io/stream/InputStream.h>
#include <util/io/stream/PrintStream.h>

#include "util/collection/Pair.h"

constexpr const char *HELP_TEXT =
#include "generated/README.md"
;

constexpr uint8_t BAR_LENGTH = 25;

/// Print the play status line with the passed time and a progress bar.
void printStatusLine(const Util::Time::Timestamp &passedTime, const Util::Time::Timestamp &totalLength) {
    const auto passedSeconds = passedTime.toSeconds();
    const auto passedMinutes = passedTime.toMinutes();
    const auto totalSeconds = totalLength.toSeconds();
    const auto totalMinutes = totalLength.toMinutes();

    const auto timeString = Util::String::format("%u:%02um/%u:%02um",
        passedMinutes, passedSeconds - passedMinutes * 60,
        totalMinutes, totalSeconds - totalMinutes * 60);

    const auto percentage = totalLength.toMilliseconds() == 0 ? 0 :
        passedTime.toMilliseconds() * 100 / totalLength.toMilliseconds();
    const auto filledBar = BAR_LENGTH * percentage / 100;

    Util::System::out << "[";
    for (size_t i = 0; i < filledBar; i++) {
        Util::System::out << "#";
    }
    for (size_t i = 0; i < BAR_LENGTH - filledBar; i++) {
        Util::System::out << "-";
    }
    Util::System::out << "] " << timeString << Util::Io::PrintStream::flush;
}

/// Parse a single line from a beep file.
/// The line must be in the format "frequency,length", where "frequency" is the frequency to play in Hz
/// and "length" is the frequency should be played in milliseconds.
Util::Pair<size_t, Util::Time::Timestamp> parseBeepLine(const Util::String &line) {
    const auto split = line.split(",");
    if (split.length() != 2) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "beep: Invalid line in beep file!");
    }

    const auto frequency = Util::String::parseNumber<size_t>(split[0]);
    const auto length = Util::String::parseNumber<size_t>(split[1]);

    return Util::Pair<size_t, Util::Time::Timestamp>(frequency, Util::Time::Timestamp::ofMilliseconds(length));
}

/// Calculate the play length of a given beep file
Util::Time::Timestamp calculateLength(const Util::Io::File &beepFile) {
    Util::Io::FileInputStream fileStream(beepFile);
    Util::Io::BufferedInputStream stream(fileStream);
    Util::Time::Timestamp length;

    auto line = stream.readLine();
    while (!line.endOfFile) {
        length += parseBeepLine(line.content).getSecond();
        line = stream.readLine();
    }

    return length;
}

int32_t main(int32_t argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.setHelpText(HELP_TEXT);

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "beep: No arguments provided!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    const Util::Io::File beepFile(arguments[0]);
    if (!beepFile.exists() || beepFile.isDirectory()) {
        Util::System::error << "beep: Failed to open '" << arguments[0] << "'!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    Util::Sound::PcSpeaker speaker(Util::Io::File("/device/speaker"));
    Util::Io::FileInputStream fileStream(beepFile);
    Util::Io::BufferedInputStream stream(fileStream);

    Util::Time::Timestamp passedTime;
    const auto songLength = calculateLength(beepFile);

    Util::Graphic::Ansi::enableKeyboardScancodes();
    Util::Graphic::Ansi::disableCursor();
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    const Util::Io::DeLayout layout;
    Util::Io::KeyDecoder keyDecoder(layout);

    Util::System::out << "Playing '" << beepFile.getName() << "'. Press <ESC> to stop." << Util::Io::PrintStream::ln;

    auto line = stream.readLine();
    while (!line.endOfFile) {
        // Exit application if ESC is pressed
        if (Util::System::in.isReadyToRead()) {
            if (keyDecoder.parseScancode(Util::System::in.read())) {
                if (keyDecoder.getKeyEvent().getScancode() == Util::Io::KeyEvent::ESC) {
                    break;
                }
            }
        }

        // Parse the current line into a frequency and length
        const auto lineData = parseBeepLine(line.content);
        const auto frequency = lineData.getFirst();
        const auto length = lineData.getSecond();

        // Update the status line
        Util::Graphic::Ansi::saveCursorPosition();
        printStatusLine(passedTime, songLength);
        Util::Graphic::Ansi::restoreCursorPosition();

        // Play the current tone
        speaker.play(frequency, length);
        passedTime += length;

        line = stream.readLine();
    }

    speaker.turnOff();

    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::moveCursorToBeginningOfPreviousLine(0);
    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::moveCursorToBeginningOfPreviousLine(0);
    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::enableCursor();

    Util::Graphic::Ansi::cleanupGraphicalApplication();
    return 0;
}