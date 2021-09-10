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
                foregroundColor = getColor(code - 30, Util::Graphic::Colors::TERM_WHITE);
                brightForeground = false;
            } else if (code < 50) {
                backgroundColor = getColor(code - 40, Util::Graphic::Colors::TERM_BLACK);
                brightBackground = false;
            } else if (code < 98) {
                foregroundColor = getColor(code - 90, Util::Graphic::Colors::TERM_WHITE);
                brightForeground = true;
            } else if (code < 108) {
                backgroundColor = getColor(code - 100, Util::Graphic::Colors::TERM_BLACK);
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
            foreground.brighten();
        }

        if (dim) {
            foreground.dim();
        }

        if (brightBackground) {
            background.brighten();
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

Graphic::Color TerminalOutputStream::getColor(uint8_t colorCode, const Util::Graphic::Color &defaultColor) {
    Util::Graphic::Color color;
    
    switch (colorCode) {
        case 0:
            color = Util::Graphic::Colors::TERM_BLACK;
            break;
        case 1:
            color = Util::Graphic::Colors::TERM_RED;
            break;
        case 2:
            color = Util::Graphic::Colors::TERM_GREEN;
            break;
        case 3:
            color = Util::Graphic::Colors::TERM_YELLOW;
            break;
        case 4:
            color = Util::Graphic::Colors::TERM_BLUE;
            break;
        case 5:
            color = Util::Graphic::Colors::TERM_MAGENTA;
            break;
        case 6:
            color = Util::Graphic::Colors::TERM_CYAN;
            break;
        case 7:
            color = Util::Graphic::Colors::TERM_WHITE;
            break;
        default:
            color = defaultColor;
    }

    return color;
}

void TerminalOutputStream::parseGraphicRendition(uint8_t code) {
    switch (code) {
        case Graphic::Ansi::GraphicRendition::NORMAL:
            foregroundColor = Graphic::Colors::TERM_WHITE;
            backgroundColor = Graphic::Colors::TERM_BLACK;
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