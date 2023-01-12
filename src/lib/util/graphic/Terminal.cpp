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

#include "lib/util/Exception.h"
#include "lib/util/async/Thread.h"
#include "lib/util/async/FunctionPointerRunnable.h"
#include "lib/util/stream/FileWriter.h"
#include "Terminal.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/stream/FileInputStream.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/io/KeyDecoder.h"

namespace Util::Graphic {

Terminal::Terminal(uint16_t columns, uint16_t rows) : outputStream(*this), columns(columns), rows(rows) {
    outputStream.connect(inputStream);
    Async::Thread::createThread("Terminal", new KeyboardRunnable(*this));
}

void Terminal::write(uint8_t c) {
    if (!ansiParsing) {
        putChar(c, foregroundColor, backgroundColor);
        return;
    }

    switch (c) {
        case Ansi::ESCAPE_SEQUENCE_START:
            isEscapeActive = true;
            break;
        case 0x07:
            handleBell();
            return;
        case 0x09:
            handleTab();
            return;
        case 0x00 ... 0x06:
        case 0x08:
        case 0x0b ... 0x1a:
        case 0x1c ... 0x1f:
            return;
        default:
            break;
    }

    if (isEscapeActive) {
        if (escapeEndCodes.contains(c)) {
            auto escapeSequence = currentEscapeSequence.substring(2, currentEscapeSequence.length());
            switch (c) {
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'f': case 'n': case 's': case 'u':
                    parseCursorEscapeSequence(escapeSequence, c);
                    break;
                case 'J': case 'K':
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
        putChar(c, foregroundColor, backgroundColor);
    }
}

void Terminal::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        write(sourceBuffer[offset + i]);
    }
}

int16_t Terminal::read() {
    return inputStream.read();
}

int32_t Terminal::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
    return inputStream.read(targetBuffer, offset, length);
}

void Terminal::handleBell() {
    Async::Thread::createThread("Terminal-Bell", new Async::FunctionPointerRunnable([](){
        auto writer = Stream::FileWriter("/device/speaker");
        writer.write("440");
        Async::Thread::sleep(Time::Timestamp::ofMilliseconds(250));
        writer.write("0");
    }));
}

void Terminal::handleTab() {
    if (getCurrentColumn() + TABULATOR_SPACES >= getColumns()) {
        setPosition(0, getCurrentRow() + 1);
    } else {
        setPosition(((getCurrentColumn() + TABULATOR_SPACES) / TABULATOR_SPACES) * TABULATOR_SPACES, getCurrentRow());
    }
}

void Terminal::parseColorEscapeSequence(const Util::Memory::String &escapeSequence) {
    const auto codes = escapeSequence.split(';');

    for (uint32_t i = 0; i < codes.length(); i++) {
        int32_t code = Util::Memory::String::parseInt(codes[i]);

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

    Util::Graphic::Color foreground = foregroundBaseColor;
    Util::Graphic::Color background = backgroundBaseColor;

    if (invert) {
        Util::Graphic::Color tmp = foreground;
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

void Terminal::parseCursorEscapeSequence(const Util::Memory::String &escapeSequence, char endCode) {
    switch (endCode) {
        case 'A': {
            const auto row = getCurrentRow() - Util::Memory::String::parseInt(escapeSequence);
            setPosition(getCurrentColumn(), row > 0 ? row : 0);
            break;
        }
        case 'B': {
            const auto row = getCurrentRow() + Util::Memory::String::parseInt(escapeSequence);
            setPosition(getCurrentColumn(), row < getRows() ? row : getRows() - 1);
            break;
        }
        case 'C': {
            const auto column = getCurrentColumn() + Util::Memory::String::parseInt(escapeSequence);
            setPosition(column < getColumns() ? column : getColumns() - 1, getCurrentRow());
            break;
        }
        case 'D': {
            const auto column = getCurrentColumn() - Util::Memory::String::parseInt(escapeSequence);
            setPosition(column > 0 ? column : 0, getCurrentRow());
            break;
        }
        case 'E': {
            const auto row = getCurrentRow() + Util::Memory::String::parseInt(escapeSequence) + 1;
            setPosition(0, row < getRows() ? row : getRows() - 1);
            break;
        }
        case 'F': {
            const auto row = getCurrentRow() - Util::Memory::String::parseInt(escapeSequence) - 1;
            setPosition(0, row > 0 ? row : 0);
            break;
        }
        case 'G': {
            auto column = Util::Memory::String::parseInt(escapeSequence);
            if (column < 0) {
                column = 0;
            } else if (column > getColumns()) {
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

            auto column = Util::Memory::String::parseInt(codes[1]);
            auto row = Util::Memory::String::parseInt(codes[0]);

            if (column < 0) {
                column = 0;
            } else if (column > getColumns()) {
                column = getColumns() - 1;
            }

            if (row < 0) {
                row = 0;
            } else if (row > getRows()) {
                row = getRows() - 1;
            }

            setPosition(column, row);
            break;
        }
        case 'n': {
            if (Util::Memory::String::parseInt(escapeSequence) == 6) {
                auto positionString = Util::Memory::String::format("\u001b[%u;%uR", getCurrentRow(), getCurrentColumn());
                outputStream.write(reinterpret_cast<const uint8_t*>(static_cast<const char*>(positionString)), 0, positionString.length());
                outputStream.flush();
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

void Terminal::parseEraseSequence(const Util::Memory::String &escapeSequence, char endCode) {
    const auto code = escapeSequence.isEmpty() ? 0 : Util::Memory::String::parseInt(escapeSequence);
    const auto column = getCurrentColumn();
    const auto row = getCurrentRow();

    switch (endCode) {
        case 'J': {
            switch (code) {
                case 0:
                    while (getCurrentColumn() < getColumns() - 1 || getCurrentRow() < getRows() - 1) {
                        putChar(' ', foregroundColor, backgroundColor);
                    }
                    break;
                case 1:
                    setPosition(0, 0);
                    while (getCurrentColumn() < column || getCurrentRow() < row) {
                        putChar(' ', foregroundColor, backgroundColor);
                    }
                    break;
                case 2:
                    clear(backgroundColor);
                    break;
                default:
                    break;
            }
            break;
        }
        case 'K': {
            switch (code) {
                case 0:
                    while (getCurrentColumn() < getColumns() - 1) {
                        putChar(' ', foregroundColor, backgroundColor);
                    }
                    break;
                case 1:
                    setPosition(0, row);
                    while (getCurrentColumn() <= column) {
                        putChar(' ', foregroundColor, backgroundColor);
                    }
                    break;
                case 2:
                    setPosition(0, row);
                    while (getCurrentColumn() < getColumns() - 1) {
                        putChar(' ', foregroundColor, backgroundColor);
                    }
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    setPosition(column, row);
}

uint16_t Terminal::getColumns() const {
    return columns;
}

uint16_t Terminal::getRows() const {
    return rows;
}

Util::Graphic::Color Terminal::getColor(uint8_t colorCode, const Util::Graphic::Color &defaultColor, const Util::Data::Array<Util::Memory::String> &codes, uint32_t &index) {
    switch (colorCode) {
        case 0:
            return Util::Graphic::Colors::BLACK;
        case 1:
            return Util::Graphic::Colors::RED;
        case 2:
            return Util::Graphic::Colors::GREEN;
        case 3:
            return Util::Graphic::Colors::YELLOW;
        case 4:
            return Util::Graphic::Colors::BLUE;
        case 5:
            return Util::Graphic::Colors::MAGENTA;
        case 6:
            return Util::Graphic::Colors::CYAN;
        case 7:
            return Util::Graphic::Colors::WHITE;
        case 8:
            return parseComplexColor(codes, ++index);
        case 9:
            return defaultColor;
        default:
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ansi: Invalid color!");
    }
}

Util::Graphic::Color Terminal::parseComplexColor(const Util::Data::Array<Util::Memory::String> &codes, uint32_t &index) {
    int32_t mode = Util::Memory::String::parseInt(codes[index++]);
    switch (mode) {
        case 2:
            return parseTrueColor(codes, index);
        case 5:
            return parse256Color(codes, index);
        default:
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ansi: Invalid color mode!");
    }
}

Util::Graphic::Color Terminal::parse256Color(const Util::Data::Array<Util::Memory::String> &codes, uint32_t &index) {
    int32_t colorIndex = Util::Memory::String::parseInt(codes[index++]);
    return Util::Graphic::Ansi::colorTable256[colorIndex];
}

Util::Graphic::Color Terminal::parseTrueColor(const Util::Data::Array<Util::Memory::String> &codes, uint32_t &index) {
    int32_t red = Util::Memory::String::parseInt(codes[index++]);;
    int32_t green = Util::Memory::String::parseInt(codes[index++]);
    int32_t blue = Util::Memory::String::parseInt(codes[index++]);

    return {static_cast<uint8_t>(red), static_cast<uint8_t>(green), static_cast<uint8_t>(blue)};
}

void Terminal::parseGraphicRendition(uint8_t code) {
    switch (code) {
        case Util::Graphic::Ansi::GraphicRendition::NORMAL:
            foregroundBaseColor = Util::Graphic::Colors::WHITE;
            backgroundBaseColor = Util::Graphic::Colors::BLACK;
            foregroundColor = Util::Graphic::Colors::WHITE;
            backgroundColor = Util::Graphic::Colors::BLACK;
            brightForeground = false;
            brightBackground = false;
            invert = false;
            bright = false;
            dim = false;
            break;
        case Util::Graphic::Ansi::GraphicRendition::BRIGHT:
            bright = true;
            break;
        case Util::Graphic::Ansi::GraphicRendition::DIM:
            dim = true;
            break;
        case Util::Graphic::Ansi::GraphicRendition::INVERT:
            invert = true;
            break;
        case Util::Graphic::Ansi::GraphicRendition::RESET_BRIGHT_DIM:
            bright = false;
            dim = false;
            break;
        case Util::Graphic::Ansi::GraphicRendition::RESET_INVERT:
            invert = false;
            break;
        default:
            break;
    }
}

void Terminal::setEcho(bool enabled) {
    echo = enabled;
}

void Terminal::setLineAggregation(bool enabled) {
    lineAggregation = enabled;
}

void Terminal::setAnsiParsing(bool enabled) {
    ansiParsing = enabled;
}

void Terminal::setKeyboardScancodes(bool enabled) {
    keyboardScancodes = enabled;
}

Terminal::TerminalPipedOutputStream::TerminalPipedOutputStream(Terminal &terminal) : terminal(terminal) {}

void Terminal::TerminalPipedOutputStream::write(uint8_t c) {
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

            auto line = lineBufferStream.getContent().substring(0, lineBufferStream.getLength() - 1);
            lineBufferStream.reset();
            lineBufferStream.write(static_cast<const uint8_t*>(line), 0, line.length());
        }
    } else {
        if (terminal.echo) {
            terminal.write(c);
        }

        if (terminal.lineAggregation) {
            lineBufferStream.write(c);

            if (c == '\n') {
                flush();
            }
        } else {
            PipedOutputStream::write(c);
        }
    }
}

void Terminal::TerminalPipedOutputStream::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        write(sourceBuffer[offset + i]);
    }
}

void Terminal::TerminalPipedOutputStream::flush() {
    PipedOutputStream::write(static_cast<uint8_t*>(lineBufferStream.getContent()), 0, lineBufferStream.getLength());
    lineBufferStream.reset();
}

Terminal::KeyboardRunnable::KeyboardRunnable(Terminal &terminal) : terminal(terminal) {}

void Terminal::KeyboardRunnable::run() {
    auto keyboardStream = Stream::FileInputStream("/device/keyboard");
    auto keyDecoder = Io::KeyDecoder();
    int16_t scancode = keyboardStream.read();

    while (scancode != -1) {
        if (terminal.keyboardScancodes) {
            terminal.outputStream.write(scancode);
            continue;
        }

        if (keyDecoder.parseScancode(scancode)) {
            auto key = keyDecoder.getCurrentKey();
            if (key.isPressed()) {
                auto c = key.getAscii();
                if (c == 0) {
                    switch (key.getScancode()) {
                        case 0x48:
                            terminal.outputStream.write(reinterpret_cast<const uint8_t *>("\u001b[1A"), 0, 4);
                            break;
                        case 0x50:
                            terminal.outputStream.write(reinterpret_cast<const uint8_t *>("\u001b[1B"), 0, 4);
                            break;
                        case 0x4D:
                            terminal.outputStream.write(reinterpret_cast<const uint8_t *>("\u001b[1C"), 0, 4);
                            break;
                        case 0x4B:
                            terminal.outputStream.write(reinterpret_cast<const uint8_t *>("\u001b[1D"), 0, 4);
                            break;
                    }
                } else {
                    if (key.getCtrl()) {
                        c &= 0x1f;
                    }

                    terminal.outputStream.write(c);
                }
            }
        }

        scancode = keyboardStream.read();
    }
}

}