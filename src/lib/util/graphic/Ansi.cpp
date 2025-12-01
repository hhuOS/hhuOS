/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#include "Ansi.h"

#include "util/base/System.h"
#include "util/collection/Array.h"
#include "util/graphic/Color.h"
#include "util/graphic/Terminal.h"
#include "util/io/file/File.h"
#include "util/io/stream/PrintStream.h"
#include "util/io/stream/InputStream.h"

namespace Util::Graphic {

const String ESCAPE_END_CODES = String::format("ABCDEFGHJKmnsu");

void Ansi::enableEcho() {
    Io::File::controlFile(Io::STANDARD_INPUT, Terminal::SET_ECHO, {true});
}

void Ansi::disableEcho() {
    Io::File::controlFile(Io::STANDARD_INPUT, Terminal::SET_ECHO, {false});
}

void Ansi::enableLineAggregation() {
    Io::File::controlFile(Io::STANDARD_INPUT, Terminal::SET_LINE_AGGREGATION, {true});
}

void Ansi::disableLineAggregation() {
    Io::File::controlFile(Io::STANDARD_INPUT, Terminal::SET_LINE_AGGREGATION, {false});
}

void Ansi::enableCursor() {
    Io::File::controlFile(Io::STANDARD_INPUT, Terminal::SET_CURSOR, {true});
}

void Ansi::disableCursor() {
    Io::File::controlFile(Io::STANDARD_INPUT, Terminal::SET_CURSOR, {false});
}

void Ansi::enableAnsiParsing() {
    Io::File::controlFile(Io::STANDARD_INPUT, Terminal::SET_ANSI_PARSING, {true});
}

void Ansi::disableAnsiParsing() {
    Io::File::controlFile(Io::STANDARD_INPUT, Terminal::SET_ANSI_PARSING, {false});
}

void Ansi::prepareGraphicalApplication(const bool enableScancodes) {
    if (enableScancodes) {
        Io::File::controlFile(Io::STANDARD_INPUT, Terminal::ENABLE_KEYBOARD_SCANCODES,
            Util::Array<uint32_t>(0));
    } else {
        Io::File::controlFile(Io::STANDARD_INPUT, Terminal::ENABLE_RAW_MODE,
            Util::Array<uint32_t>(0));
    }

    disableCursor();
}

void Ansi::cleanupGraphicalApplication() {
    Io::File::controlFile(Io::STANDARD_INPUT, Terminal::ENABLE_CANONICAL_MODE,
        Util::Array<uint32_t>(0));

    enableCursor();
}

void Ansi::enableRawMode() {
    Io::File::controlFile(Io::STANDARD_INPUT, Terminal::ENABLE_RAW_MODE,
        Util::Array<uint32_t>(0));
}

void Ansi::enableCanonicalMode() {
    Io::File::controlFile(Io::STANDARD_INPUT, Terminal::ENABLE_CANONICAL_MODE,
        Util::Array<uint32_t>(0));
}

String Ansi::foreground8BitColor(const uint8_t colorIndex) {
    return String::format("\u001b[38;5;%um", colorIndex);
}

String Ansi::background8BitColor(const uint8_t colorIndex) {
    return String::format("\u001b[48;5;%um", colorIndex);
}

String Ansi::foreground24BitColor(const Color &color) {
    return String::format("\u001b[38;2;%u;%u;%um", color.getRed(), color.getGreen(), color.getBlue());
}

String Ansi::background24BitColor(const Color &color) {
    return String::format("\u001b[48;2;%u;%u;%um", color.getRed(), color.getGreen(), color.getBlue());
}

void Ansi::setForegroundColor(const AnsiColor color, const bool bright) {
    System::out << "\u001b[" << Io::PrintStream::dec << color + (bright ? 90 : 30) << "m" << Io::PrintStream::flush;
}

void Ansi::setBackgroundColor(const AnsiColor color, const bool bright) {
    System::out << "\u001b[" << Io::PrintStream::dec << color + (bright ? 100 : 40) << "m" << Io::PrintStream::flush;
}

void Ansi::setForegroundColor(const uint8_t colorIndex) {
    System::out << "\u001b[38;5;" << Io::PrintStream::dec << colorIndex << "m" << Io::PrintStream::flush;
}

void Ansi::setBackgroundColor(const uint8_t colorIndex) {
    System::out << "\u001b[48;5;" << Io::PrintStream::dec << colorIndex << "m" << Io::PrintStream::flush;
}

void Ansi::setForegroundColor(const Color &color) {
    System::out << "\u001b[38;2;" << Io::PrintStream::dec << color.getRed() << ";" << color.getGreen() << ";" << color.getBlue() << "m" << Io::PrintStream::flush;
}

void Ansi::setBackgroundColor(const Color &color) {
    System::out << "\u001b[48;2;" << Io::PrintStream::dec << color.getRed() << ";" << color.getGreen() << ";" << color.getBlue() << "m" << Io::PrintStream::flush;
}

void Ansi::resetForegroundColor() {
    System::out << "\u001b[39m" << Io::PrintStream::flush;
}

void Ansi::resetBackgroundColor() {
    System::out << "\u001b[49m" << Io::PrintStream::flush;
}

void Ansi::resetColorsAndEffects() {
    System::out << "\u001b[0m" << Io::PrintStream::flush;
}

void Ansi::setPosition(const CursorPosition &position) {
    System::out << "\u001b[" << Io::PrintStream::dec << position.row << ";" << position.column << "H" <<
        Io::PrintStream::flush;
}

void Ansi::moveCursorUp(const uint16_t lines) {
    System::out << "\u001b[" << Io::PrintStream::dec << lines << "A" << Io::PrintStream::flush;
}

void Ansi::moveCursorDown(const uint16_t lines) {
    System::out << "\u001b[" << Io::PrintStream::dec << lines << "B" << Io::PrintStream::flush;
}

void Ansi::moveCursorRight(const uint16_t columns) {
    System::out << "\u001b[" << Io::PrintStream::dec << columns << "C" << Io::PrintStream::flush;
}

void Ansi::moveCursorLeft(const uint16_t columns) {
    System::out << "\u001b[" << Io::PrintStream::dec << columns << "D" << Io::PrintStream::flush;
}

void Ansi::moveCursorToBeginningOfNextLine(const uint16_t offset) {
    System::out << "\u001b[" << Io::PrintStream::dec << offset << "E" << Io::PrintStream::flush;
}

void Ansi::moveCursorToBeginningOfPreviousLine(const uint16_t offset) {
    System::out << "\u001b[" << Io::PrintStream::dec << offset << "F" << Io::PrintStream::flush;
}

void Ansi::setColumn(const uint16_t column) {
    System::out << "\u001b[" << Io::PrintStream::dec << column << "G" << Io::PrintStream::flush;
}

void Ansi::saveCursorPosition() {
    System::out << "\u001b[s" << Io::PrintStream::flush;
}

void Ansi::restoreCursorPosition() {
    System::out << "\u001b[u" << Io::PrintStream::flush;
}

void Ansi::clearScreen() {
    System::out << "\u001b[2J" << Io::PrintStream::flush;
}

void Ansi::clearScreenFromCursor() {
    System::out << "\u001b[0J" << Io::PrintStream::flush;
}

void Ansi::clearScreenToCursor() {
    System::out << "\u001b[1J" << Io::PrintStream::flush;
}

void Ansi::clearLine() {
    System::out << "\u001b[2K" << Io::PrintStream::flush;
}

void Ansi::clearLineFromCursor() {
    System::out << "\u001b[0K" << Io::PrintStream::flush;
}

void Ansi::clearLineToCursor() {
    System::out << "\u001b[1K" << Io::PrintStream::flush;
}

Ansi::CursorPosition Ansi::getCursorPosition() {
    System::out << "\u001b[6n" << Io::PrintStream::flush;

    String positionString;
    char currentChar = System::in.read();
    while (currentChar != 'R') {
        positionString += currentChar;
        currentChar = System::in.read();
    }

    auto split = positionString.substring(2).split(";");
    return CursorPosition{String::parseNumber<uint16_t>(split[1]), String::parseNumber<uint16_t>(split[0])};
}

Ansi::CursorPosition Ansi::getCursorLimits() {
    const auto position = getCursorPosition();
    setPosition(CursorPosition{UINT16_MAX, UINT16_MAX});

    const auto size = getCursorPosition();
    setPosition(position);

    return size;
}

int16_t Ansi::readChar(Io::InputStream &stream) {
    char input = stream.read();
    if (input == ESCAPE_SEQUENCE_START) {
        String escapeSequence = input;

        do {
            input = stream.read();
            escapeSequence += input;
        } while (!ESCAPE_END_CODES.contains(input));

        switch (input) {
            case 'A':
                return UP;
            case 'B':
                return DOWN;
            case 'C':
                return RIGHT;
            case 'D':
                return LEFT;
            case 'H':
                return HOME;
            case 'F':
                return END;
            default:
                enableAnsiParsing();
                System::out << escapeSequence << Io::PrintStream::flush;
                disableAnsiParsing();
                return readChar(stream);
        }
    }

    return input;
}

}