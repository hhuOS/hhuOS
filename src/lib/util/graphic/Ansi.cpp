/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Ansi.h"

#include "lib/util/base/System.h"
#include "Terminal.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/io/stream/InputStream.h"

namespace Util::Graphic {

const String Ansi::escapeEndCodes = Util::String::format("ABCDEFGHJKmnsu");

const Graphic::Color Ansi::colorTable256[256] = {
        // 16 predefined colors, matching the 4-bit ANSI colors
        Colors::BLACK, Colors::RED, Colors::GREEN, Colors::YELLOW,
        Colors::BLUE, Colors::MAGENTA, Colors::CYAN, Colors::WHITE,
        Colors::BLACK.bright(), Colors::RED.bright(), Colors::GREEN.bright(), Colors::YELLOW.bright(),
        Colors::BLUE.bright(), Colors::MAGENTA.bright(), Colors::CYAN.bright(), Colors::WHITE.bright(),

        // 216 colors
        Graphic::Color(0, 0, 0), Graphic::Color(0, 0, 95), Graphic::Color(0, 0, 135),
        Graphic::Color(0, 0, 175), Graphic::Color(0, 0, 215), Graphic::Color(0, 0, 255),

        Graphic::Color(0, 95, 0), Graphic::Color(0, 95, 95), Graphic::Color(0, 95, 135),
        Graphic::Color(0, 95, 175), Graphic::Color(0, 95, 215), Graphic::Color(0, 95, 255),

        Graphic::Color(0, 135, 0), Graphic::Color(0, 135, 95), Graphic::Color(0, 135, 135),
        Graphic::Color(0, 135, 175), Graphic::Color(0, 135, 215), Graphic::Color(0, 135, 255),

        Graphic::Color(0, 175, 0), Graphic::Color(0, 175, 95), Graphic::Color(0, 175, 135),
        Graphic::Color(0, 175, 175), Graphic::Color(0, 175, 215), Graphic::Color(0, 175, 255),

        Graphic::Color(0, 215, 0), Graphic::Color(0, 215, 95), Graphic::Color(0, 215, 135),
        Graphic::Color(0, 215, 175), Graphic::Color(0, 215, 215), Graphic::Color(0, 215, 255),

        Graphic::Color(0, 255, 0), Graphic::Color(0, 255, 95), Graphic::Color(0, 255, 135),
        Graphic::Color(0, 255, 175), Graphic::Color(0, 255, 215), Graphic::Color(0, 255, 255),

        Graphic::Color(95, 0, 0), Graphic::Color(95, 0, 95), Graphic::Color(95, 0, 135),
        Graphic::Color(95, 0, 175), Graphic::Color(95, 0, 215), Graphic::Color(95, 0, 255),

        Graphic::Color(95, 95, 0), Graphic::Color(95, 95, 95), Graphic::Color(95, 95, 135),
        Graphic::Color(95, 95, 175), Graphic::Color(95, 95, 215), Graphic::Color(95, 95, 255),

        Graphic::Color(95, 135, 0), Graphic::Color(95, 135, 95), Graphic::Color(95, 135, 135),
        Graphic::Color(95, 135, 175), Graphic::Color(95, 135, 215), Graphic::Color(95, 135, 255),

        Graphic::Color(95, 175, 0), Graphic::Color(95, 175, 95), Graphic::Color(95, 175, 135),
        Graphic::Color(95, 175, 175), Graphic::Color(95, 175, 215), Graphic::Color(95, 175, 255),

        Graphic::Color(95, 215, 0), Graphic::Color(95, 215, 95), Graphic::Color(95, 215, 135),
        Graphic::Color(95, 215, 175), Graphic::Color(95, 215, 215), Graphic::Color(95, 215, 255),

        Graphic::Color(95, 255, 0), Graphic::Color(95, 255, 95), Graphic::Color(95, 255, 135),
        Graphic::Color(95, 255, 175), Graphic::Color(95, 255, 215), Graphic::Color(95, 255, 255),

        Graphic::Color(135, 0, 0), Graphic::Color(135, 0, 95), Graphic::Color(135, 0, 135),
        Graphic::Color(135, 0, 175), Graphic::Color(135, 0, 215), Graphic::Color(135, 0, 255),

        Graphic::Color(135, 95, 0), Graphic::Color(135, 95, 95), Graphic::Color(135, 95, 135),
        Graphic::Color(135, 95, 175), Graphic::Color(135, 95, 215), Graphic::Color(135, 95, 255),

        Graphic::Color(135, 135, 0), Graphic::Color(135, 135, 95), Graphic::Color(135, 135, 135),
        Graphic::Color(135, 135, 175), Graphic::Color(135, 135, 215), Graphic::Color(135, 135, 255),

        Graphic::Color(135, 175, 0), Graphic::Color(135, 175, 95), Graphic::Color(135, 175, 135),
        Graphic::Color(135, 175, 175), Graphic::Color(135, 175, 215), Graphic::Color(135, 175, 255),

        Graphic::Color(135, 215, 0), Graphic::Color(135, 215, 95), Graphic::Color(135, 215, 135),
        Graphic::Color(135, 215, 175), Graphic::Color(135, 215, 215), Graphic::Color(135, 215, 255),

        Graphic::Color(135, 255, 0), Graphic::Color(135, 255, 95), Graphic::Color(135, 255, 135),
        Graphic::Color(135, 255, 175), Graphic::Color(135, 255, 215), Graphic::Color(135, 255, 255),

        Graphic::Color(175, 0, 0), Graphic::Color(175, 0, 95), Graphic::Color(175, 0, 135),
        Graphic::Color(175, 0, 175), Graphic::Color(175, 0, 215), Graphic::Color(175, 0, 255),

        Graphic::Color(175, 95, 0), Graphic::Color(175, 95, 95), Graphic::Color(175, 95, 135),
        Graphic::Color(175, 95, 175), Graphic::Color(175, 95, 215), Graphic::Color(175, 95, 255),

        Graphic::Color(175, 135, 0), Graphic::Color(175, 135, 95), Graphic::Color(175, 135, 135),
        Graphic::Color(175, 135, 175), Graphic::Color(175, 135, 215), Graphic::Color(175, 135, 255),

        Graphic::Color(175, 175, 0), Graphic::Color(175, 175, 95), Graphic::Color(175, 175, 135),
        Graphic::Color(175, 175, 175), Graphic::Color(175, 175, 215), Graphic::Color(175, 175, 255),

        Graphic::Color(175, 215, 0), Graphic::Color(175, 215, 95), Graphic::Color(175, 215, 135),
        Graphic::Color(175, 215, 175), Graphic::Color(175, 215, 215), Graphic::Color(175, 215, 255),

        Graphic::Color(175, 255, 0), Graphic::Color(175, 255, 95), Graphic::Color(175, 255, 135),
        Graphic::Color(175, 255, 175), Graphic::Color(175, 255, 215), Graphic::Color(175, 255, 255),

        Graphic::Color(215, 0, 0), Graphic::Color(215, 0, 95), Graphic::Color(215, 0, 135),
        Graphic::Color(215, 0, 175), Graphic::Color(215, 0, 215), Graphic::Color(215, 0, 255),

        Graphic::Color(215, 95, 0), Graphic::Color(215, 95, 95), Graphic::Color(215, 95, 135),
        Graphic::Color(215, 95, 175), Graphic::Color(215, 95, 215), Graphic::Color(215, 95, 255),

        Graphic::Color(215, 135, 0), Graphic::Color(215, 135, 95), Graphic::Color(215, 135, 135),
        Graphic::Color(215, 135, 175), Graphic::Color(215, 135, 215), Graphic::Color(215, 135, 255),

        Graphic::Color(215, 175, 0), Graphic::Color(215, 175, 95), Graphic::Color(215, 175, 135),
        Graphic::Color(215, 175, 175), Graphic::Color(215, 175, 215), Graphic::Color(215, 175, 255),

        Graphic::Color(215, 215, 0), Graphic::Color(215, 215, 95), Graphic::Color(215, 215, 135),
        Graphic::Color(215, 215, 175), Graphic::Color(215, 215, 215), Graphic::Color(215, 215, 255),

        Graphic::Color(215, 255, 0), Graphic::Color(215, 255, 95), Graphic::Color(215, 255, 135),
        Graphic::Color(215, 255, 175), Graphic::Color(215, 255, 215), Graphic::Color(215, 255, 255),

        Graphic::Color(255, 0, 0), Graphic::Color(255, 0, 95), Graphic::Color(255, 0, 135),
        Graphic::Color(255, 0, 175), Graphic::Color(255, 0, 215), Graphic::Color(255, 0, 255),

        Graphic::Color(255, 95, 0), Graphic::Color(255, 95, 95), Graphic::Color(255, 95, 135),
        Graphic::Color(255, 95, 175), Graphic::Color(255, 95, 215), Graphic::Color(255, 95, 255),

        Graphic::Color(255, 135, 0), Graphic::Color(255, 135, 95), Graphic::Color(255, 135, 135),
        Graphic::Color(255, 135, 175), Graphic::Color(255, 135, 215), Graphic::Color(255, 135, 255),

        Graphic::Color(255, 175, 0), Graphic::Color(255, 175, 95), Graphic::Color(255, 175, 135),
        Graphic::Color(255, 175, 175), Graphic::Color(255, 175, 215), Graphic::Color(255, 175, 255),

        Graphic::Color(255, 215, 0), Graphic::Color(255, 215, 95), Graphic::Color(255, 215, 135),
        Graphic::Color(255, 215, 175), Graphic::Color(255, 215, 215), Graphic::Color(255, 215, 255),

        Graphic::Color(255, 255, 0), Graphic::Color(255, 255, 95), Graphic::Color(255, 255, 135),
        Graphic::Color(255, 255, 175), Graphic::Color(255, 255, 215), Graphic::Color(255, 255, 255),

        // 24 grayscale Graphic::Colors
        Graphic::Color(8, 8, 8), Graphic::Color(18, 18, 18), Graphic::Color(28, 28, 28),
        Graphic::Color(38, 38, 38), Graphic::Color(48, 48, 48), Graphic::Color(58, 58, 58),
        Graphic::Color(68, 68, 68), Graphic::Color(78, 78, 78), Graphic::Color(88, 88, 88),
        Graphic::Color(98, 98, 98), Graphic::Color(108, 108, 108), Graphic::Color(118, 118, 118),
        Graphic::Color(128, 128, 128), Graphic::Color(138, 138, 138), Graphic::Color(148, 148, 148),
        Graphic::Color(158, 158, 158), Graphic::Color(168, 168, 168), Graphic::Color(178, 178, 178),
        Graphic::Color(188, 188, 188), Graphic::Color(198, 198, 198), Graphic::Color(208, 208, 208),
        Graphic::Color(218, 218, 218), Graphic::Color(228, 228, 228), Graphic::Color(238, 238, 238)
};

void Ansi::enableEcho() {
    Io::File::control(Io::STANDARD_INPUT, Graphic::Terminal::SET_ECHO, {true});
}

void Ansi::disableEcho() {
    Io::File::control(Io::STANDARD_INPUT, Graphic::Terminal::SET_ECHO, {false});
}

void Ansi::enableLineAggregation() {
    Io::File::control(Io::STANDARD_INPUT, Graphic::Terminal::SET_LINE_AGGREGATION, {true});
}

void Ansi::disableLineAggregation() {
    Io::File::control(Io::STANDARD_INPUT, Graphic::Terminal::SET_LINE_AGGREGATION, {false});
}

void Ansi::enableCursor() {
    Io::File::control(Util::Io::STANDARD_INPUT, Util::Graphic::Terminal::SET_CURSOR, {true});
}

void Ansi::disableCursor() {
    Io::File::control(Util::Io::STANDARD_INPUT, Util::Graphic::Terminal::SET_CURSOR, {false});
}

void Ansi::enableAnsiParsing() {
    Io::File::control(Util::Io::STANDARD_INPUT, Util::Graphic::Terminal::SET_ANSI_PARSING, {true});
}

void Ansi::disableAnsiParsing() {
    Io::File::control(Util::Io::STANDARD_INPUT, Util::Graphic::Terminal::SET_ANSI_PARSING, {false});
}

void Ansi::prepareGraphicalApplication(bool enableScancodes) {
    if (enableScancodes) {
        Io::File::control(Io::STANDARD_INPUT, Graphic::Terminal::ENABLE_KEYBOARD_SCANCODES, {});
    } else {
        Io::File::control(Io::STANDARD_INPUT, Graphic::Terminal::ENABLE_RAW_MODE, {});
    }
    disableCursor();
}

void Ansi::cleanupGraphicalApplication() {
    Io::File::control(Io::STANDARD_INPUT, Graphic::Terminal::ENABLE_CANONICAL_MODE, {});
    enableCursor();
}

void Ansi::enableRawMode() {
    Io::File::control(Io::STANDARD_INPUT, Graphic::Terminal::ENABLE_RAW_MODE, {});
}

void Ansi::enableCanonicalMode() {
    Io::File::control(Io::STANDARD_INPUT, Graphic::Terminal::ENABLE_CANONICAL_MODE, {});
}

String Ansi::foreground8BitColor(uint8_t colorIndex) {
    return String::format("\u001b[38;5;%um", colorIndex);
}

String Ansi::background8BitColor(uint8_t colorIndex) {
    return String::format("\u001b[48;5;%um", colorIndex);
}

String Ansi::foreground24BitColor(const Graphic::Color &color) {
    return String::format("\u001b[38;2;%u;%u;%um", color.getRed(), color.getGreen(), color.getBlue());
}

String Ansi::background24BitColor(const Graphic::Color &color) {
    return String::format("\u001b[48;2;%u;%u;%um", color.getRed(), color.getGreen(), color.getBlue());
}

void Ansi::setForegroundColor(Color color, bool bright) {
    System::out << "\u001b[" << Io::PrintStream::dec << color + (bright ? 90 : 30) << "m" << Io::PrintStream::flush;
}

void Ansi::setBackgroundColor(Color color, bool bright) {
    System::out << "\u001b[" << Io::PrintStream::dec << color + (bright ? 100 : 40) << "m" << Io::PrintStream::flush;
}

void Ansi::setForegroundColor(uint8_t colorIndex) {
    System::out << "\u001b[38;5;" << Io::PrintStream::dec << colorIndex << "m" << Io::PrintStream::flush;
}

void Ansi::setBackgroundColor(uint8_t colorIndex) {
    System::out << "\u001b[48;5;" << Io::PrintStream::dec << colorIndex << "m" << Io::PrintStream::flush;
}

void Ansi::setForegroundColor(const Graphic::Color &color) {
    System::out << "\u001b[38;2;" << Io::PrintStream::dec << color.getRed() << ";" << color.getGreen() << ";" << color.getBlue() << "m" << Io::PrintStream::flush;
}

void Ansi::setBackgroundColor(const Graphic::Color &color) {
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
    System::out << "\u001b[" << Io::PrintStream::dec << position.row << ";" << position.column << "H" << Io::PrintStream::flush;
}

void Ansi::moveCursorUp(uint16_t lines) {
    System::out << "\u001b[" << Io::PrintStream::dec << lines << "A" << Io::PrintStream::flush;
}

void Ansi::moveCursorDown(uint16_t lines) {
    System::out << "\u001b[" << Io::PrintStream::dec << lines << "B" << Io::PrintStream::flush;
}

void Ansi::moveCursorRight(uint16_t columns) {
    System::out << "\u001b[" << Io::PrintStream::dec << columns << "C" << Io::PrintStream::flush;
}

void Ansi::moveCursorLeft(uint16_t columns) {
    System::out << "\u001b[" << Io::PrintStream::dec << columns << "D" << Io::PrintStream::flush;
}

void Ansi::moveCursorToBeginningOfNextLine(uint16_t offset) {
    System::out << "\u001b[" << Io::PrintStream::dec << offset << "E" << Io::PrintStream::flush;
}

void Ansi::moveCursorToBeginningOfPreviousLine(uint16_t offset) {
    System::out << "\u001b[" << Io::PrintStream::dec << offset << "F" << Io::PrintStream::flush;
}

void Ansi::setColumn(uint16_t column) {
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
    return {static_cast<uint16_t>(String::parseInt(split[1])), static_cast<uint16_t>(String::parseInt(split[0]))};
}

Ansi::CursorPosition Ansi::getCursorLimits() {
    auto position = getCursorPosition();
    setPosition(CursorPosition{UINT16_MAX, UINT16_MAX});

    auto size = getCursorPosition();
    setPosition(position);

    return size;
}

int16_t Ansi::readChar() {
    char input = System::in.read();
    if (input == ESCAPE_SEQUENCE_START) {
        String escapeSequence = input;

        do {
            input = System::in.read();
            escapeSequence += input;
        } while (!escapeEndCodes.contains(input));

        switch (input) {
            case 'A':
                return KEY_UP;
            case 'B':
                return KEY_DOWN;
            case 'C':
                return KEY_RIGHT;
            case 'D':
                return KEY_LEFT;
            default:
                enableAnsiParsing();
                System::out << escapeSequence << Io::PrintStream::flush;
                disableAnsiParsing();
                return readChar();
        }
    }

    return input;
}

}