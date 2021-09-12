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

#include <lib/util/Exception.h>
#include <lib/util/graphic/Ansi.h>
#include <lib/util/memory/String.h>
#include "TerminalOutputStream.h"

namespace Util::Stream  {

TerminalOutputStream::TerminalOutputStream(Graphic::Terminal &terminal) : terminal(terminal) {
    terminal.setForegroundColor(foregroundColor);
    terminal.setBackgroundColor(backgroundColor);
}

void TerminalOutputStream::write(uint8_t c) {
    if (c == '\u001b') {
        isEscapeActive = true;
    }

    if (isEscapeActive && c == Graphic::Ansi::ESCAPE_END) {
        uint32_t index = 0;
        while (true) {
            int32_t code = extractNextAnsiCode(index);
            if (code == -1) {
                break;
            }

            if (code < 30) {
                parseGraphicRendition(code);
            } else if (code < 40) {
                foregroundColor = getColor(code - 30, Util::Graphic::Colors::WHITE, index);
                brightForeground = false;
            } else if (code < 50) {
                backgroundColor = getColor(code - 40, Util::Graphic::Colors::BLACK, index);
                brightBackground = false;
            } else if (code < 98) {
                foregroundColor = getColor(code - 90, Util::Graphic::Colors::WHITE, index);
                brightForeground = true;
            } else if (code < 108) {
                backgroundColor = getColor(code - 100, Util::Graphic::Colors::BLACK, index);
                brightBackground = true;
            }
        }

        Util::Graphic::Color foreground = foregroundColor;
        Util::Graphic::Color background = backgroundColor;

        if (invert) {
            Graphic::Color tmp = foreground;
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

        terminal.setForegroundColor(foreground);
        terminal.setBackgroundColor(background);

        isEscapeActive = false;
        escapeCodeIndex = 0;
    } else if (isEscapeActive) {
        currentEscapeCode[escapeCodeIndex++] = c;
    } else {
        terminal.putChar(c);
    }
}

void TerminalOutputStream::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    if (offset < 0 || length < 0) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "OutputStream: Negative offset or size!");
    }

    for (uint32_t i = 0; i < length; i++) {
        write(sourceBuffer[offset + i]);
    }
}

int32_t TerminalOutputStream::extractNextAnsiCode(uint32_t &index) const {
    while (!Util::Memory::String::isNumeric(currentEscapeCode[index]) && index < escapeCodeIndex) {
        index++;
    }

    if (index >= escapeCodeIndex) {
        return -1;
    }

    char code[sizeof(currentEscapeCode)]{};
    for (uint32_t i = 0; Util::Memory::String::isNumeric(currentEscapeCode[index]) && index < escapeCodeIndex; index++, i++) {
        code[i] = currentEscapeCode[index];
    }

    return Util::Memory::String::parseInt(code);
}

Graphic::Color TerminalOutputStream::getColor(uint8_t colorCode, const Util::Graphic::Color &defaultColor, uint32_t &index) {
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
            return parseComplexColor(index);
        case 9:
            return defaultColor;
        default:
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ansi: Invalid color!");
    }
}

Graphic::Color TerminalOutputStream::parseComplexColor(uint32_t &index) {
    int32_t mode = extractNextAnsiCode(index);
    switch (mode) {
        case -1:
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ansi: Missing color code!");
        case 2:
            return parseTrueColor(index);
        case 5:
            return parse256Color(index);
        default:
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ansi: Invalid color mode!");
    }
}

Graphic::Color TerminalOutputStream::parse256Color(uint32_t &index) {
    int32_t colorIndex = extractNextAnsiCode(index);
    if (colorIndex == -1) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ansi: Missing index for 8-bit color!");
    }

    return Graphic::Ansi::get8BitColor(colorIndex);
}

Graphic::Color TerminalOutputStream::parseTrueColor(uint32_t &index) {
    int32_t red = extractNextAnsiCode(index);
    int32_t green = extractNextAnsiCode(index);
    int32_t blue = extractNextAnsiCode(index);

    if (red == -1 || green == -1 || blue == -1) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ansi: Missing value for 24-bit color!");
    }

    return { static_cast<uint8_t>(red), static_cast<uint8_t>(green), static_cast<uint8_t>(blue) };
}

void TerminalOutputStream::parseGraphicRendition(uint8_t code) {
    switch (code) {
        case Graphic::Ansi::GraphicRendition::NORMAL:
            foregroundColor = Graphic::Colors::WHITE;
            backgroundColor = Graphic::Colors::BLACK;
            invert = false;
            bright = false;
            break;
        case Graphic::Ansi::GraphicRendition::BRIGHT:
            bright = true;
            break;
        case Graphic::Ansi::GraphicRendition::DIM:
            dim = true;
            break;
        case Graphic::Ansi::GraphicRendition::INVERT:
            invert = true;
            break;
        case Graphic::Ansi::GraphicRendition::RESET_BRIGHT_DIM:
            bright = false;
            dim = false;
            break;
        case Graphic::Ansi::GraphicRendition::RESET_INVERT:
            invert = false;
            break;
        default:
            break;
    }
}

}