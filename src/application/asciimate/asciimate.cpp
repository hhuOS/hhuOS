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
#include <stddef.h>

#include <util/base/ArgumentParser.h>
#include <util/base/String.h>
#include <util/base/System.h>
#include <util/collection/Array.h>
#include <util/graphic/Ansi.h>
#include <util/io/file/File.h>
#include <util/io/stream/InputStream.h>
#include <util/io/stream/FileInputStream.h>
#include <util/io/stream/BufferedInputStream.h>
#include <util/io/stream/PrintStream.h>
#include <util/io/key/KeyDecoder.h>
#include <util/io/key/layout/DeLayout.h>
#include <util/async/Thread.h>
#include <util/time/Timestamp.h>

const char *HELP_TEXT =
#include "generated/README.md"
;

int32_t main(int32_t argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.setHelpText(HELP_TEXT);

    argumentParser.addArgument("fps", false, "f");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "asciimate: No arguments provided!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    auto file = Util::Io::File(arguments[0]);
    if (!file.exists() || file.isDirectory()) {
        Util::System::error << "asciimate: Failed to open '" << arguments[0] << "'!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    Util::Io::FileInputStream fileInputStream(file);
    Util::Io::BufferedInputStream inputStream(fileInputStream);

    const auto fpsString = argumentParser.getArgument("fps", "15");
    const auto fps = Util::String::parseNumber<size_t>(fpsString);
    const auto timePerFrame = Util::Time::Timestamp::ofMicroseconds(1000000 / fps);

    const auto frameInfo = inputStream.readLine().content.split(",");
    const auto rows = Util::String::parseNumber<uint16_t>(frameInfo[0]);
    const auto columns = Util::String::parseNumber<uint16_t>(frameInfo[1]);

    // Adjust cursor position so the frame fits in the terminal
    const auto terminalResolution = Util::Graphic::Ansi::getCursorLimits();
    const auto cursorPosition = Util::Graphic::Ansi::getCursorPosition();
    if (rows > terminalResolution.row - cursorPosition.row) {
        const auto newRow = static_cast<uint16_t>(terminalResolution.row < rows ? 0 : terminalResolution.row - rows);
        Util::Graphic::Ansi::setPosition(Util::Graphic::Ansi::CursorPosition{0, newRow});
    }

    const auto maxRows = rows > terminalResolution.row ? terminalResolution.row : rows;
    const auto maxColumns = columns > terminalResolution.column ? terminalResolution.column : columns;

    Util::Graphic::Ansi::enableKeyboardScancodes();
    Util::Graphic::Ansi::disableCursor();
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    const Util::Io::DeLayout layout;
    Util::Io::KeyDecoder keyDecoder(layout);

    while (true) {
        // Exit application if ESC is pressed
        if (Util::System::in.isReadyToRead()) {
            if (keyDecoder.parseScancode(Util::System::in.read())) {
                if (keyDecoder.getKeyEvent().getScancode() == Util::Io::KeyEvent::ESC) {
                    break;
                }
            }
        }

        // Each frame starts with a line containing the delay.
        // The delay is given in frames (e.g. a delay of 2 means that the frame should be displayed for 2 frames,
        // which equals 2 * timePerFrame).
        const auto delayLine = inputStream.readLine();
        if (delayLine.content.isEmpty() || delayLine.endOfFile) {
            // End of file reached, or invalid format
            break;
        }

        // Calculate display time for current frame
        const auto delay = Util::String::parseNumber<size_t>(delayLine.content);
        const auto targetFrameTime = timePerFrame * delay;

        // Save cursor position at the beginning of the frame
        Util::Graphic::Ansi::saveCursorPosition();
        const auto frameStartTime = Util::Time::Timestamp::getSystemTime();

        // Read the frame line by line and draw it to the terminal
        for (size_t i = 0; i < rows; i++) {
            const auto line = inputStream.readLine();
            if (i < maxRows) {
                Util::System::out << line.content.substring(0, maxColumns) << Util::Io::PrintStream::ln;
            }
        }

        // Flush the terminal and wait for the specified delay before drawing the next frame
        Util::System::out << Util::Io::PrintStream::flush;

        const auto frameTime = Util::Time::Timestamp::getSystemTime() - frameStartTime;
        if (frameTime < targetFrameTime) {
            Util::Async::Thread::sleep(targetFrameTime - frameTime);
        }

        // Restore cursor position for drawing the next frame
        Util::Graphic::Ansi::restoreCursorPosition();
    }

    // Clean up terminal
    Util::Graphic::Ansi::saveCursorPosition();
    for (size_t i = 0; i < rows; i++) {
        Util::Graphic::Ansi::clearLineFromCursor();
        Util::Graphic::Ansi::moveCursorToBeginningOfNextLine(0);
    }
    Util::Graphic::Ansi::restoreCursorPosition();

    Util::Graphic::Ansi::cleanupGraphicalApplication();
    return 0;
}