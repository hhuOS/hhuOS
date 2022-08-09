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
#include "Terminal.h"

namespace Util::Graphic {

Terminal::Terminal(uint16_t columns, uint16_t rows) : outputStream(*this), columns(columns), rows(rows) {
    outputStream.connect(inputStream);
}

void Terminal::write(uint8_t c) {
    if (c == Ansi::ESCAPE_SEQUENCE_START) {
        isEscapeActive = true;
    } else if (c == '\t') {
        if (getCurrentColumn() + TABULATOR_SPACES >= getColumns()) {
            setPosition(0, getCurrentRow() + 1);
        } else {
            setPosition(((getCurrentColumn() + TABULATOR_SPACES) / TABULATOR_SPACES) * TABULATOR_SPACES, getCurrentRow());
        }

        return;
    } else if (c < 0x20 && c != '\n') {
        return;
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

Util::Stream::PipedOutputStream &Terminal::getPipedOutputStream() {
    return outputStream;
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

Terminal::TerminalPipedOutputStream::TerminalPipedOutputStream(Terminal &terminal, uint32_t lineBufferSize) : terminal(terminal), lineBufferStream(LINE_BUFFER_SIZE), lineBuffer(new uint8_t[lineBufferSize]) {}

Terminal::TerminalPipedOutputStream::~TerminalPipedOutputStream() {
    delete[] lineBuffer;
}

void Terminal::TerminalPipedOutputStream::write(uint8_t c) {
    if (c == '\b') {
        if (!lineBufferStream.isEmpty()) {
            auto column = terminal.getCurrentColumn();
            auto row = terminal.getCurrentRow();
            if (column == 0) {
                column = terminal.getColumns() - 1;
                row--;
            } else {
                column--;
            }

            terminal.putChar(' ', terminal.foregroundColor, terminal.backgroundColor);
            terminal.setPosition(column, row);
            terminal.putChar(' ', terminal.foregroundColor, terminal.backgroundColor);
            terminal.setPosition(column, row);

            auto lineLength = lineBufferStream.getSize() - 1;
            lineBufferStream.getContent(lineBuffer, lineLength);
            lineBufferStream.reset();
            lineBufferStream.write(lineBuffer, 0, lineLength);
        }
    } else if (lineBufferStream.getSize() < LINE_BUFFER_SIZE) {
        lineBufferStream.write(c);
        terminal.write(c);

        if (c == '\n') {
            flush();
        }
    }
}

void Terminal::TerminalPipedOutputStream::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        write(sourceBuffer[offset + i]);
    }
}

void Terminal::TerminalPipedOutputStream::flush() {
    lineBufferStream.getContent(lineBuffer, lineBufferStream.getSize());
    PipedOutputStream::write(lineBuffer, 0, lineBufferStream.getSize());
    lineBufferStream.reset();
}

}