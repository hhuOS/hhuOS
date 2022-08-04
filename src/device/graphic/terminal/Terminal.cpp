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

namespace Device::Graphic {

Terminal::Terminal(uint16_t columns, uint16_t rows) : columns(columns), rows(rows) {}

void Terminal::write(uint8_t c) {
    if (c == '\b') {
        auto column = getCurrentColumn();
        auto row = getCurrentRow();
        if (column == 0) {
            column = columns - 1;
            row--;
        } else {
            column--;
        }


        putChar(' ', foregroundColor, backgroundColor);
        setPosition(column, row);
        putChar(' ', foregroundColor, backgroundColor);
        setPosition(column, row);

        return;
    }

    // Ignore carriage return
    if (c == '\r') {
        return;
    }

    if (c == '\u001b') {
        isEscapeActive = true;
    }

    if (isEscapeActive) {
        if (escapeEndCodes.contains(c)) {
            auto escapeSequence = currentEscapeSequence.substring(2, currentEscapeSequence.length());
            switch (c) {
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 's': case 'u':
                    parseSetCursorEscapeSequence(escapeSequence, c);
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

void Terminal::parseSetCursorEscapeSequence(const Util::Memory::String &escapeSequence, char endCode) {
    switch (endCode) {
        case 'A': {
            const auto row = getCurrentRow() - Util::Memory::String::parseInt(escapeSequence);
            setPosition(getCurrentColumn(), row);
            break;
        }
        case 'B': {
            const auto row = getCurrentRow() + Util::Memory::String::parseInt(escapeSequence);
            setPosition(getCurrentColumn(), row);
            break;
        }
        case 'C': {
            const auto column = getCurrentColumn() + Util::Memory::String::parseInt(escapeSequence);
            setPosition(column, getCurrentRow());
            break;
        }
        case 'D': {
            const auto column = getCurrentColumn() - Util::Memory::String::parseInt(escapeSequence);
            setPosition(column, getCurrentRow());
            break;
        }
        case 'E': {
            const auto row = getCurrentRow() + Util::Memory::String::parseInt(escapeSequence) + 1;
            setPosition(0, row);
            break;
        }
        case 'F': {
            const auto row = getCurrentRow() - Util::Memory::String::parseInt(escapeSequence) - 1;
            setPosition(0, row);
            break;
        }
        case 'G': {
            const auto column = Util::Memory::String::parseInt(escapeSequence);
            setPosition(column, getCurrentRow());
            break;
        }
        case 'H': {
            if (escapeSequence.isEmpty()) {
                setPosition(0, 0);
                return;
            }

            auto codes = escapeSequence.split(";");
            if (codes.length() < 2) {
                return;
            }

            setPosition(Util::Memory::String::parseInt(codes[1]), Util::Memory::String::parseInt(codes[0]));
            break;
        }
        case 's': {
            savedColumn = getCurrentColumn();
            saveRow = getCurrentRow();
            break;
        }
        case 'u': {
            setPosition(savedColumn, saveRow);
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
                    while (getCurrentColumn() < getColumns() || getCurrentRow() < getRows()) {
                        putChar(' ', foregroundColor, backgroundColor);
                    }
                    break;
                case 1:
                    setPosition(0, 0);
                    while (getCurrentColumn() <= column || getCurrentRow() <= row) {
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
                    while (getCurrentColumn() < getColumns()) {
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
                    while (getCurrentColumn() < getColumns()) {
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
    return Util::Graphic::Ansi::get8BitColor(colorIndex);
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

}