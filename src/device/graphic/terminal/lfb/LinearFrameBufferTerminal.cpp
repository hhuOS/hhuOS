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

#include <lib/util/memory/Address.h>
#include "LinearFrameBufferTerminal.h"

namespace Device::Graphic {

LinearFrameBufferTerminal::LinearFrameBufferTerminal(Util::Graphic::LinearFrameBuffer *lfb, Util::Graphic::Font &font) :
        Terminal(lfb->getResolutionX() / font.getCharWidth(), lfb->getResolutionY() / font.getCharHeight()),
        lfb(*lfb), scroller(*lfb), pixelDrawer(*lfb), stringDrawer(pixelDrawer), font(font) {
    LinearFrameBufferTerminal::clear(Util::Graphic::Colors::BLACK);
}

LinearFrameBufferTerminal::~LinearFrameBufferTerminal() {
    delete &lfb;
}

void LinearFrameBufferTerminal::putChar(char c, const Util::Graphic::Color &foregroundColor, const Util::Graphic::Color &backgroundColor) {
    if (c == '\n') {
        stringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), ' ', Util::Graphic::Colors::INVISIBLE, Util::Graphic::Colors::BLACK);
        currentRow++;
        currentColumn = 0;
    } else {
        stringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), c, foregroundColor, backgroundColor);
        currentColumn++;
    }

    if (currentColumn >= getColumns()) {
        currentRow++;
        currentColumn = 0;
    }

    if (currentRow >= getRows()) {
        scroller.scrollUp(font.getCharHeight());
        currentColumn = 0;
        currentRow = getRows() - 1 ;
    }

    updateCursorPosition();
}

void LinearFrameBufferTerminal::clear(const Util::Graphic::Color &backgroundColor) {
    lfb.clear();

    currentRow = 0;
    currentColumn = 0;
    updateCursorPosition();
}

void LinearFrameBufferTerminal::setPosition(uint16_t column, uint16_t row) {
    currentColumn = column;
    currentRow = row;
}

void LinearFrameBufferTerminal::updateCursorPosition() {
    stringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), ' ', Util::Graphic::Colors::INVISIBLE, Util::Graphic::Colors::WHITE);
}

}