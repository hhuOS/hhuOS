/*
 * Copyright (C) 2021 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HHUOS_TERMINAL_H
#define HHUOS_TERMINAL_H

#include "LinearFrameBuffer.h"
#include "Colors.h"
#include "PixelDrawer.h"
#include "StringDrawer.h"
#include "Fonts.h"

namespace Util::Graphic {

class Terminal {

public:

    explicit Terminal(LinearFrameBuffer &lfb, Font &font = Fonts::TERMINAL_FONT, char cursor = '_');

    Terminal(const Terminal &copy) = delete;

    Terminal &operator=(const Terminal &other) = delete;

    ~Terminal() = default;

    void putChar(char c);

    void putString(const char *string);

    void setPosition(uint16_t x, uint16_t y);

    void setForegroundColor(Color &color);

    void setBackgroundColor(Color &color);

private:

    void scrollUp();

    const uint16_t columns;
    const uint16_t rows;
    const char cursor;

    LinearFrameBuffer &lfb;
    PixelDrawer pixelDrawer;
    StringDrawer stringDrawer;

    Font &font;
    Color fgColor = Colors::TERM_WHITE;
    Color bgColor = Colors::TERM_BLACK;
    uint16_t currentColumn = 0;
    uint16_t currentRow = 0;
};

}

#endif
