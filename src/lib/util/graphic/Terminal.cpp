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

#include "Terminal.h"

#include "util/base/Panic.h"
#include "util/async/Thread.h"
#include "util/async/BasicRunnable.h"
#include "util/graphic/Ansi.h"
#include "util/io/key/Key.h"
#include "util/io/key/KeyDecoder.h"
#include "util/io/stream/FileInputStream.h"
#include "util/io/stream/FileOutputStream.h"
#include "util/io/stream/PrintStream.h"
#include "util/time/Timestamp.h"

namespace Util {
namespace Graphic {

Terminal::Terminal(const uint16_t columns, const uint16_t rows) :
    terminalStream(*this), columns(columns), rows(rows)
{
    ansiOutputStream.connect(ansiInputStream);
    Async::Thread::createThread("Terminal", new KeyboardRunnable(*this));
}

bool Terminal::write(const uint8_t c) {
    writeLock.acquire();

    if (!ansiParsing) {
        if (c == '\n') {
            clear(foregroundColor, backgroundColor, getCurrentColumn(), getCurrentRow(),
                columns, getCurrentRow());
            setPosition(0, getCurrentRow() + 1);
        } else {
            putChar(c, foregroundColor, backgroundColor);
        }

        writeLock.release();
        return true;
    }

    if (c == Ansi::ESCAPE_SEQUENCE_START) {
        isEscapeActive = true;
    }

    if (isEscapeActive) {
        if (escapeEndCodes.contains(c)) {
            const auto escapeSequence = currentEscapeSequence.substring(
                2, currentEscapeSequence.length());

            switch (c) {
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'f':
                case 'n':
                case 's':
                case 'u':
                    parseCursorEscapeSequence(escapeSequence, c);
                    break;
                case 'J':
                case 'K':
                    parseEraseSequence(escapeSequence, c);
                    break;
                case 'm':
                    parseColorEscapeSequence(escapeSequence);
                    break;
                default:
                    break;
            }

            isEscapeActive = false;
            currentEscapeSequence = "";
        } else {
            currentEscapeSequence += c;
        }
    } else {
        if (c == 0x07) {
            handleBell();
        } else if (c == 0x09) {
            handleTab();
        } else if (c == '\n') {
            clear(foregroundColor, backgroundColor, getCurrentColumn(), getCurrentRow(),
                columns, getCurrentRow());
            setPosition(0, getCurrentRow() + 1);
        } else if (c <= 0x06 || c == 0x08 || (c >= 0x0b && c <= 0x1a) || (c >= 0x1c && c <= 0x1f)) {
            // Unimplemented control characters
        } else {
            putChar(c, foregroundColor, backgroundColor);
        }
    }

    return writeLock.releaseAndReturn(true);
}

size_t Terminal::write(const uint8_t *sourceBuffer, const size_t offset, const size_t length) {
    for (uint32_t i = 0; i < length; i++) {
        if (!write(sourceBuffer[offset + i])) {
            return i;
        }
    }

    return length;
}

int16_t Terminal::read() {
    uint8_t c;
    read(&c, 0, 1);

    return c;
}

int16_t Terminal::peek() {
    writeLock.acquire();
    if (ansiInputStream.isReadyToRead()) {
        return writeLock.releaseAndReturn(ansiInputStream.peek());
    }
    writeLock.release();

    return terminalStream.peek();
}

int32_t Terminal::read(uint8_t *targetBuffer, const size_t offset, const size_t length) {
    int32_t readBytes = 0;

    // Try to read from ANSI input stream first
    writeLock.acquire();
    if (ansiInputStream.isReadyToRead()) {
        readBytes = ansiInputStream.read(targetBuffer, offset, length);
    }
    writeLock.release();

    // Read from key input stream if ANSI input stream did not provide enough data
    if (readBytes == 0 || (static_cast<uint32_t>(readBytes) < length && terminalStream.isReadyToRead())) {
        readBytes += terminalStream.read(targetBuffer, offset + readBytes, length - readBytes);
    }

    return readBytes;
}

bool Terminal::isReadyToRead() {
    writeLock.acquire();
    const auto ret = ansiInputStream.isReadyToRead() || terminalStream.isReadyToRead();

    return writeLock.releaseAndReturn(ret);
}

void Terminal::handleBell() {
    Async::Thread::createThread("Terminal-Bell", new Async::BasicRunnable([]{
        Io::FileOutputStream stream("/device/speaker");
        Io::PrintStream printStream(stream);

        printStream << "440" << Io::PrintStream::flush;
        Async::Thread::sleep(Time::Timestamp::ofMilliseconds(250));
        printStream << "0" << Io::PrintStream::flush;
    }));
}

void Terminal::handleTab() {
    if (getCurrentColumn() + TABULATOR_SPACES >= getColumns()) {
        setPosition(0, getCurrentRow() + 1);
    } else {
        setPosition((getCurrentColumn() + TABULATOR_SPACES) / TABULATOR_SPACES * TABULATOR_SPACES,
            getCurrentRow());
    }
}

void Terminal::parseColorEscapeSequence(const String &escapeSequence) {
    const auto codes = escapeSequence.split(';');

    for (uint32_t i = 0; i < codes.length(); i++) {
        const auto code = String::parseNumber<size_t>(codes[i]);

        if (code < 30) {
            parseGraphicRendition(code);
        } else if (code < 40) {
            foregroundBaseColor = getColor(code - 30, Util::Graphic::Colors::WHITE, codes, i);
            brightForeground = false;
        } else if (code < 50) {
            backgroundBaseColor = getColor(code - 40, Util::Graphic::Colors::BLACK, codes, i);
            brightBackground = false;
        } else if (code < 98) {
            foregroundBaseColor = getColor(code - 90, Util::Graphic::Colors::WHITE, codes, i);
            brightForeground = true;
        } else if (code < 108) {
            backgroundBaseColor = getColor(code - 100, Util::Graphic::Colors::BLACK, codes, i);
            brightBackground = true;
        }
    }

    Color foreground = foregroundBaseColor;
    Color background = backgroundBaseColor;

    if (invert) {
        const auto tmp = foreground;
        foreground = background;
        background = tmp;
    }

    if (bright || brightForeground) {
        foreground = foreground.bright();
    }

    if (dim) {
        foreground = foreground.dim();
    }

    if (brightBackground) {
        background = background.bright();
    }

    foregroundColor = foreground;
    backgroundColor = background;
}

void Terminal::parseCursorEscapeSequence(const String &escapeSequence, const char endCode) {
    switch (endCode) {
        case 'A': {
            const auto row = getCurrentRow() - String::parseNumber<uint16_t>(escapeSequence);
            setPosition(getCurrentColumn(), row > 0 ? row : 0);
            break;
        }
        case 'B': {
            const auto row = getCurrentRow() + String::parseNumber<uint16_t>(escapeSequence);
            setPosition(getCurrentColumn(), row < getRows() ? row : getRows() - 1);
            break;
        }
        case 'C': {
            const auto column = getCurrentColumn() + String::parseNumber<uint16_t>(escapeSequence);
            setPosition(column < getColumns() ? column : getColumns() - 1, getCurrentRow());
            break;
        }
        case 'D': {
            const auto column = getCurrentColumn() - String::parseNumber<uint16_t>(escapeSequence);
            setPosition(column > 0 ? column : 0, getCurrentRow());
            break;
        }
        case 'E': {
            const auto row = getCurrentRow() + String::parseNumber<uint16_t>(escapeSequence) + 1;
            setPosition(0, row < getRows() ? row : getRows() - 1);
            break;
        }
        case 'F': {
            const auto row = getCurrentRow() - String::parseNumber<uint16_t>(escapeSequence) - 1;
            setPosition(0, row > 0 ? row : 0);
            break;
        }
        case 'G': {
            auto column = String::parseNumber<uint16_t>(escapeSequence);
            if (column > getColumns()) {
                column = getColumns() - 1;
            }

            setPosition(column, getCurrentRow());
            break;
        }
        case 'H': case 'f': {
            if (escapeSequence.isEmpty()) {
                setPosition(0, 0);
                return;
            }

            auto codes = escapeSequence.split(";");
            if (codes.length() < 2) {
                return;
            }

            auto column = String::parseNumber<uint16_t>(codes[1]);
            auto row = String::parseNumber<uint16_t>(codes[0]);

            if (column > getColumns()) {
                column = getColumns() - 1;
            }

            if (row > getRows()) {
                row = getRows() - 1;
            }

            setPosition(column, row);
            break;
        }
        case 'n': {
            if (String::parseNumber<size_t>(escapeSequence) == 6) {
                const auto positionString = String::format("\u001b[%u;%uR", getCurrentRow(), getCurrentColumn());
                ansiOutputStream.write(reinterpret_cast<const uint8_t*>(static_cast<const char*>(positionString)),
                    0, positionString.length());
                ansiOutputStream.flush();
            }
            break;
        }
        case 's': {
            savedColumn = getCurrentColumn();
            savedRow = getCurrentRow();
            break;
        }
        case 'u': {
            setPosition(savedColumn, savedRow);
            break;
        }
        default:
            break;
    }
}

void Terminal::parseEraseSequence(const String &escapeSequence, const char endCode) {
    const auto code = escapeSequence.isEmpty() ? 0 : String::parseNumber<size_t>(escapeSequence);

    switch (endCode) {
        case 'J': {
            switch (code) {
                case 0:
                    // Clear screen from cursor
                    clear(foregroundColor, backgroundColor, getCurrentColumn(), getCurrentRow(),
                        columns - 1, rows - 1);
                    break;
                case 1:
                    // Clear screen to cursor
                    clear(foregroundColor, backgroundColor, 0, 0,
                        getCurrentColumn(), getCurrentRow());
                    break;
                case 2:
                    // Clear screen
                    clear();
                    break;
                default:
                    break;
            }
            break;
        }
        case 'K': {
            switch (code) {
                case 0:
                    // Clear line from cursor
                    clear(foregroundColor, backgroundColor, getCurrentColumn(), getCurrentRow(),
                        columns - 1, getCurrentRow());
                    break;
                case 1:
                    // Clear line to cursor
                    clear(foregroundColor, backgroundColor, 0, getCurrentRow(),
                        getCurrentColumn(), getCurrentRow());
                    break;
                case 2:
                    // Clear line
                    clear(foregroundColor, backgroundColor, 0, getCurrentRow(),
                        columns - 1, getCurrentRow());
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}

Color Terminal::getColor(const uint8_t colorCode, const Color &defaultColor, const Array<String> &codes,
    uint32_t &index)
{
    switch (colorCode) {
        case 0:
            return Colors::BLACK;
        case 1:
            return Colors::RED;
        case 2:
            return Colors::GREEN;
        case 3:
            return Colors::YELLOW;
        case 4:
            return Colors::BLUE;
        case 5:
            return Colors::MAGENTA;
        case 6:
            return Colors::CYAN;
        case 7:
            return Colors::WHITE;
        case 8:
            return parseComplexColor(codes, ++index);
        case 9:
            return defaultColor;
        default:
            Util::Panic::fire(Panic::INVALID_ARGUMENT, "Ansi: Invalid color!");
    }
}

Color Terminal::parseComplexColor(const Array<String> &codes, uint32_t &index) {
    const auto mode = String::parseNumber<size_t>(codes[index++]);
    switch (mode) {
        case 2:
            return parseTrueColor(codes, index);
        case 5:
            return parse256Color(codes, index);
        default:
            Util::Panic::fire(Panic::INVALID_ARGUMENT, "Ansi: Invalid color mode!");
    }
}

Color Terminal::parse256Color(const Array<String> &codes, uint32_t &index) {
    const auto colorIndex = String::parseNumber<uint8_t>(codes[index++]);
    return Ansi::COLOR_TABLE_256[colorIndex];
}

Color Terminal::parseTrueColor(const Array<String> &codes, uint32_t &index) {
    const auto red = String::parseNumber<uint8_t>(codes[index++]);;
    const auto green = String::parseNumber<uint8_t>(codes[index++]);
    const auto blue = String::parseNumber<uint8_t>(codes[index++]);

    return Color(red, green, blue);
}

void Terminal::parseGraphicRendition(const uint8_t code) {
    switch (code) {
        case Ansi::GraphicRendition::NORMAL:
            foregroundBaseColor = Colors::WHITE;
            backgroundBaseColor = Colors::BLACK;
            foregroundColor = Colors::WHITE;
            backgroundColor = Colors::BLACK;
            brightForeground = false;
            brightBackground = false;
            invert = false;
            bright = false;
            dim = false;
            break;
        case Ansi::GraphicRendition::BRIGHT:
            bright = true;
            break;
        case Ansi::GraphicRendition::DIM:
            dim = true;
            break;
        case Ansi::GraphicRendition::INVERT:
            invert = true;
            break;
        case Ansi::GraphicRendition::RESET_BRIGHT_DIM:
            bright = false;
            dim = false;
            break;
        case Ansi::GraphicRendition::RESET_INVERT:
            invert = false;
            break;
        default:
            break;
    }
}

void Terminal::clear() {
    clear(foregroundColor, backgroundColor, 0, 0, getColumns() - 1, getRows() - 1);
    setPosition(0, 0);
}

Terminal::TerminalStream::TerminalStream(Terminal &terminal) :
    FilterInputStream(keyInputStream), FilterOutputStream(keyOutputStream), terminal(terminal)
{
    keyOutputStream.connect(keyInputStream);
}

bool Terminal::TerminalStream::write(const uint8_t c) {
    if (terminal.ansiParsing && c == '\b') {
        if (!lineBufferStream.isEmpty()) {
            auto column = terminal.getCurrentColumn();
            auto row = terminal.getCurrentRow();
            if (column == 0) {
                column = terminal.getColumns() - 1;
                row--;
            } else {
                column--;
            }

            if (terminal.echo) {
                terminal.putChar(' ', terminal.foregroundColor, terminal.backgroundColor);
                terminal.setPosition(column, row);
                terminal.putChar(' ', terminal.foregroundColor, terminal.backgroundColor);
                terminal.setPosition(column, row);
            }

            const auto line = lineBufferStream.getContent().substring(
                0, lineBufferStream.getPosition() - 1);

            lineBufferStream.reset();
            lineBufferStream.write(static_cast<const uint8_t*>(line), 0, line.length());
        }

        return true;
    }

    if (terminal.echo) {
        terminal.write(c);
    }

    if (terminal.lineAggregation) {
        if (!lineBufferStream.write(c)) {
            return false;
        }

        if (c == '\n') {
            flush();
        }

        return true;
    }

    return FilterOutputStream::write(c);
}

size_t Terminal::TerminalStream::write(const uint8_t *sourceBuffer, size_t offset, size_t length) {
    for (uint32_t i = 0; i < length; i++) {
        if (!write(sourceBuffer[offset + i])) {
            return i;
        }
    }

    return length;
}

uint32_t Terminal::TerminalStream::flush() {
    const auto written = FilterOutputStream::write(
        static_cast<const uint8_t*>(lineBufferStream.getContent()), 0, lineBufferStream.getPosition());

    lineBufferStream.reset();
    return written;
}

Terminal::KeyboardRunnable::KeyboardRunnable(Terminal &terminal) : terminal(terminal) {}

void Terminal::KeyboardRunnable::run() {
    Io::FileInputStream keyboardStream("/device/keyboard");
    int16_t scancode = keyboardStream.read();

    while (scancode != -1) {
        terminal.writeLock.acquire();
        while (terminal.isEscapeActive) {
            terminal.writeLock.release();
            Async::Thread::yield();
            terminal.writeLock.acquire();
        }

        if (terminal.keyboardScancodes) {
            terminal.terminalStream.write(scancode);
        } else if (terminal.keyDecoder.parseScancode(scancode)) {
            const auto key = terminal.keyDecoder.getCurrentKey();
            if (key.isPressed()) {
                auto c = key.getAscii();
                if (c == 0) {
                    switch (key.getScancode()) {
                        case Io::Key::UP:
                            terminal.terminalStream.write(
                                reinterpret_cast<const uint8_t*>("\u001b[1A"), 0, 4);
                            break;
                        case Io::Key::DOWN:
                            terminal.terminalStream.write(
                                reinterpret_cast<const uint8_t*>("\u001b[1B"), 0, 4);
                            break;
                        case Io::Key::RIGHT:
                            terminal.terminalStream.write(
                                reinterpret_cast<const uint8_t*>("\u001b[1C"), 0, 4);
                            break;
                        case Io::Key::LEFT:
                            terminal.terminalStream.write(
                                reinterpret_cast<const uint8_t*>("\u001b[1D"), 0, 4);
                            break;
                        case Io::Key::END:
                            terminal.terminalStream.write(
                                reinterpret_cast<const uint8_t*>("\u001b[1F"), 0, 4);
                            break;
                        case Io::Key::HOME:
                            terminal.terminalStream.write(
                                reinterpret_cast<const uint8_t*>("\u001b[1H"), 0, 4);
                            break;
                        case Io::Key::DEL:
                            terminal.terminalStream.write(0x7f);
                        default:
                            break;
                    }
                } else {
                    if (key.getCtrl()) {
                        c &= 0x1f;
                    }

                    terminal.terminalStream.write(c);
                }
            }
        }

        terminal.writeLock.release();
        scancode = keyboardStream.read();
    }
}

}
}