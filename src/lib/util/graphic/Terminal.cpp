/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/base/Exception.h"
#include "lib/util/async/Thread.h"
#include "lib/util/async/FunctionPointerRunnable.h"
#include "Terminal.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/Key.h"
#include "lib/util/io/stream/FileOutputStream.h"
#include "lib/util/io/stream/PrintStream.h"

namespace Util {
namespace Io {
class KeyboardLayout;
}  // namespace Io
}  // namespace Util

namespace Util::Graphic {

Terminal::Terminal(uint16_t columns, uint16_t rows) : outputStream(*this), columns(columns), rows(rows) {
    outputStream.connect(inputStream);
    Async::Thread::createThread("Terminal", new KeyboardRunnable(*this));
}

void Terminal::write(uint8_t c) {
    if (!ansiParsing) {
        if (c == '\n') {
            clear(foregroundColor, backgroundColor, getCurrentColumn(), getCurrentRow(), columns, getCurrentRow());
            setPosition(0, getCurrentRow() + 1);
        } else {
            putChar(c, foregroundColor, backgroundColor);
        }
        return;
    }

    if (c == Ansi::ESCAPE_SEQUENCE_START) {
        isEscapeActive = true;
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
        if (c == 0x07) {
            handleBell();
        } else if (c == 0x09) {
            handleTab();
        } else if (c == '\n') {
            clear(foregroundColor, backgroundColor, getCurrentColumn(), getCurrentRow(), columns, getCurrentRow());
            setPosition(0, getCurrentRow() + 1);
        } else if (c <= 0x06 || c == 0x08 || (c >= 0x0b && c <= 0x1a) || (c >= 0x1c && c <= 0x1f)) {
            return;
        } else {
            putChar(c, foregroundColor, backgroundColor);
        }
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

int16_t Terminal::peek() {
	return inputStream.peek();
}

int32_t Terminal::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
    return inputStream.read(targetBuffer, offset, length);
}

bool Terminal::isReadyToRead() {
    return inputStream.isReadyToRead();
}

void Terminal::handleBell() {
    Async::Thread::createThread("Terminal-Bell", new Async::FunctionPointerRunnable([](){
        auto stream = Io::FileOutputStream("/device/speaker");
        auto printStream = Io::PrintStream(stream);

        printStream << "440" << Util::Io::PrintStream::flush;
        Async::Thread::sleep(Time::Timestamp::ofMilliseconds(250));
        printStream << "0" << Util::Io::PrintStream::flush;
    }));
}

void Terminal::handleTab() {
    if (getCurrentColumn() + TABULATOR_SPACES >= getColumns()) {
        setPosition(0, getCurrentRow() + 1);
    } else {
        setPosition(((getCurrentColumn() + TABULATOR_SPACES) / TABULATOR_SPACES) * TABULATOR_SPACES, getCurrentRow());
    }
}

void Terminal::parseColorEscapeSequence(const Util::String &escapeSequence) {
    const auto codes = escapeSequence.split(';');

    for (uint32_t i = 0; i < codes.length(); i++) {
        int32_t code = Util::String::parseInt(codes[i]);

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

void Terminal::parseCursorEscapeSequence(const Util::String &escapeSequence, char endCode) {
    switch (endCode) {
        case 'A': {
            const auto row = getCurrentRow() - Util::String::parseInt(escapeSequence);
            setPosition(getCurrentColumn(), row > 0 ? row : 0);
            break;
        }
        case 'B': {
            const auto row = getCurrentRow() + Util::String::parseInt(escapeSequence);
            setPosition(getCurrentColumn(), row < getRows() ? row : getRows() - 1);
            break;
        }
        case 'C': {
            const auto column = getCurrentColumn() + Util::String::parseInt(escapeSequence);
            setPosition(column < getColumns() ? column : getColumns() - 1, getCurrentRow());
            break;
        }
        case 'D': {
            const auto column = getCurrentColumn() - Util::String::parseInt(escapeSequence);
            setPosition(column > 0 ? column : 0, getCurrentRow());
            break;
        }
        case 'E': {
            const auto row = getCurrentRow() + Util::String::parseInt(escapeSequence) + 1;
            setPosition(0, row < getRows() ? row : getRows() - 1);
            break;
        }
        case 'F': {
            const auto row = getCurrentRow() - Util::String::parseInt(escapeSequence) - 1;
            setPosition(0, row > 0 ? row : 0);
            break;
        }
        case 'G': {
            auto column = Util::String::parseInt(escapeSequence);
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

            auto column = Util::String::parseInt(codes[1]);
            auto row = Util::String::parseInt(codes[0]);

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
            if (Util::String::parseInt(escapeSequence) == 6) {
                auto positionString = Util::String::format("\u001b[%u;%uR", getCurrentRow(), getCurrentColumn());
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

void Terminal::parseEraseSequence(const Util::String &escapeSequence, char endCode) {
    const auto code = escapeSequence.isEmpty() ? 0 : Util::String::parseInt(escapeSequence);

    switch (endCode) {
        case 'J': {
            switch (code) {
                case 0:
                    // Clear screen from cursor
                    clear(foregroundColor, backgroundColor, getCurrentColumn(), getCurrentRow(), columns - 1, rows - 1);
                    break;
                case 1:
                    // Clear screen to cursor
                    clear(foregroundColor, backgroundColor, 0, 0, getCurrentColumn(), getCurrentRow());
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
                    clear(foregroundColor, backgroundColor, getCurrentColumn(), getCurrentRow(), columns - 1, getCurrentRow());
                    break;
                case 1:
                    // Clear line to cursor
                    clear(foregroundColor, backgroundColor, 0, getCurrentRow(), getCurrentColumn(), getCurrentRow());
                    break;
                case 2:
                    // Clear line
                    clear(foregroundColor, backgroundColor, 0, getCurrentRow(), columns - 1, getCurrentRow());
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

uint16_t Terminal::getColumns() const {
    return columns;
}

uint16_t Terminal::getRows() const {
    return rows;
}

Util::Graphic::Color Terminal::getColor(uint8_t colorCode, const Util::Graphic::Color &defaultColor, const Util::Array<Util::String> &codes, uint32_t &index) {
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

Util::Graphic::Color Terminal::parseComplexColor(const Util::Array<Util::String> &codes, uint32_t &index) {
    int32_t mode = Util::String::parseInt(codes[index++]);
    switch (mode) {
        case 2:
            return parseTrueColor(codes, index);
        case 5:
            return parse256Color(codes, index);
        default:
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ansi: Invalid color mode!");
    }
}

Util::Graphic::Color Terminal::parse256Color(const Util::Array<Util::String> &codes, uint32_t &index) {
    int32_t colorIndex = Util::String::parseInt(codes[index++]);
    return Util::Graphic::Ansi::colorTable256[colorIndex];
}

Util::Graphic::Color Terminal::parseTrueColor(const Util::Array<Util::String> &codes, uint32_t &index) {
    int32_t red = Util::String::parseInt(codes[index++]);;
    int32_t green = Util::String::parseInt(codes[index++]);
    int32_t blue = Util::String::parseInt(codes[index++]);

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

void Terminal::setKeyboardLayout(Io::KeyboardLayout *layout) {
    keyDecoder.setLayout(layout);
}

void Terminal::clear() {
    clear(foregroundColor, backgroundColor, 0, 0, getColumns() - 1, getRows() - 1);
    setPosition(0, 0);
}

const Color& Terminal::getForegroundColor() const {
    return foregroundColor;
}

const Color& Terminal::getBackgroundColor() const {
    return backgroundColor;
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
    auto keyboardStream = Io::FileInputStream("/device/keyboard");
    int16_t scancode = keyboardStream.read();

    while (scancode != -1) {
        if (terminal.keyboardScancodes) {
            terminal.outputStream.write(scancode);
        } else if (terminal.keyDecoder.parseScancode(scancode)) {
            auto key = terminal.keyDecoder.getCurrentKey();
            if (key.isPressed()) {
                auto c = key.getAscii();
                if (c == 0) {
                    switch (key.getScancode()) {
                        case Io::Key::UP:
                            terminal.outputStream.write(reinterpret_cast<const uint8_t *>("\u001b[1A"), 0, 4);
                            break;
                        case Io::Key::DOWN:
                            terminal.outputStream.write(reinterpret_cast<const uint8_t*>("\u001b[1B"), 0, 4);
                            break;
                        case Io::Key::RIGHT:
                            terminal.outputStream.write(reinterpret_cast<const uint8_t*>("\u001b[1C"), 0, 4);
                            break;
                        case Io::Key::LEFT:
                            terminal.outputStream.write(reinterpret_cast<const uint8_t*>("\u001b[1D"), 0, 4);
                            break;
                        case Io::Key::END:
                            terminal.outputStream.write(reinterpret_cast<const uint8_t*>("\u001b[1F"), 0, 4);
                            break;
                        case Io::Key::HOME:
                            terminal.outputStream.write(reinterpret_cast<const uint8_t*>("\u001b[1H"), 0, 4);
                            break;
                        case Io::Key::DEL:
                            terminal.outputStream.write(0x7f);
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