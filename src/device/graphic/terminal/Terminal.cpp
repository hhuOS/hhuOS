/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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
#include "lib/util/graphic/Ansi.h"
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

    if (c == '\u001b') {
        isEscapeActive = true;
    }

    if (isEscapeActive && c == Util::Graphic::Ansi::ESCAPE_END) {
        if (currentEscapeCode[1] == '[') {
            currentEscapeCode = currentEscapeCode.substring(2, currentEscapeCode.length());
        } else {
            currentEscapeCode = currentEscapeCode.substring(1, currentEscapeCode.length());
        }

        const auto codes = currentEscapeCode.split(';');

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

        isEscapeActive = false;
        currentEscapeCode = "";
    } else if (isEscapeActive) {
        currentEscapeCode += c;
    } else {
        putChar(c, foregroundColor, backgroundColor);
    }
}

void Terminal::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    if (offset < 0 || length < 0) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "OutputStream: Negative offset or size!");
    }

    for (uint32_t i = 0; i < length; i++) {
        write(sourceBuffer[offset + i]);
    }
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