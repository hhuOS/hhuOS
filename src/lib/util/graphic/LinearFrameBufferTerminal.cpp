/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "CursorRunnable.h"
#include "lib/util/base/Address.h"
#include "LinearFrameBufferTerminal.h"
#include "lib/util/graphic/Font.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/async/Thread.h"

namespace Util::Graphic {

LinearFrameBufferTerminal::LinearFrameBufferTerminal(Util::Graphic::LinearFrameBuffer *lfb, Util::Graphic::Font &font, char cursor) :
        Terminal(lfb->getResolutionX() / font.getCharWidth(), lfb->getResolutionY() / font.getCharHeight()),
        characterBuffer(new Character[getColumns() * getRows()]), lfb(*lfb), pixelDrawer(*lfb), stringDrawer(pixelDrawer), shadowLfb(*lfb, false),
        shadowPixelDrawer(shadowLfb), shadowStringDrawer(shadowPixelDrawer), shadowScroller(shadowLfb, false), font(font), cursor(cursor) {
    LinearFrameBufferTerminal::clear(Util::Graphic::Colors::BLACK);
    LinearFrameBufferTerminal::setCursor(true);
}

LinearFrameBufferTerminal::~LinearFrameBufferTerminal() {
    LinearFrameBufferTerminal::setCursor(false);
    delete &lfb;
    delete[] characterBuffer;
}

void LinearFrameBufferTerminal::putChar(char c, const Util::Graphic::Color &foregroundColor, const Util::Graphic::Color &backgroundColor) {
    while (!cursorLock.tryAcquire()) {
        cursorLock.release();
    }

    if (c == '\n') {
        stringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), ' ', Util::Graphic::Colors::INVISIBLE, Util::Graphic::Colors::BLACK);
        currentRow++;
        currentColumn = 0;
    } else {
        const auto bufferIndex = currentRow * getColumns() + currentColumn;
        characterBuffer[bufferIndex].value = c;
        characterBuffer[bufferIndex].foregroundColor = foregroundColor;
        characterBuffer[bufferIndex].backgroundColor = backgroundColor;

        stringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), c, foregroundColor, backgroundColor);
        shadowStringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), c, foregroundColor, backgroundColor);
        currentColumn++;
    }

    if (currentColumn >= getColumns()) {
        currentRow++;
        currentColumn = 0;
    }

    if (currentRow >= getRows()) {
        scrollUp();
        currentColumn = 0;
        currentRow = getRows() - 1;
    }

    cursorLock.release();
}

void LinearFrameBufferTerminal::clear(const Util::Graphic::Color &backgroundColor) {
    while (!cursorLock.tryAcquire()) {
        cursorLock.release();
    }

    uint32_t size = getRows() * getColumns();
    for (uint32_t i = 0; i < size; i++) {
        characterBuffer[i].clear();
    }

    shadowLfb.clear();
    shadowLfb.flush();
    currentRow = 0;
    currentColumn = 0;

    cursorLock.release();
}

void LinearFrameBufferTerminal::setPosition(uint16_t column, uint16_t row) {
    while (!cursorLock.tryAcquire()) {
        cursorLock.release();
    }

    auto character = characterBuffer[currentRow * getColumns() + currentColumn];
    stringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), character.value, character.foregroundColor, character.backgroundColor);

    currentColumn = column;
    currentRow = row;

    while (currentRow >= getRows()) {
        scrollUp();
        currentRow--;
    }

    cursorLock.release();
}

void LinearFrameBufferTerminal::setCursor(bool enabled) {
    cursorLock.acquire();

    if (enabled) {
        shadowLfb.flush();

        if (cursorRunnable != nullptr) {
            cursorLock.release();
            return;
        }

        cursorRunnable = new CursorRunnable(*this, cursor);
        Util::Async::Thread::createThread("Cursor", cursorRunnable);
    } else if (cursorRunnable != nullptr) {
        cursorRunnable->stop();
        cursorRunnable = nullptr;
    }

    cursorLock.release();
}

void LinearFrameBufferTerminal::scrollUp() {
    auto characterAddress = Util::Address<uint32_t>(characterBuffer);
    characterAddress.copyRange(characterAddress.add(getColumns() * sizeof(Character)), getColumns() * (getRows() - 1) * sizeof(Character));

    for (uint32_t i = 0; i < getColumns(); i++) {
        characterBuffer[getColumns() * (getRows() - 1) + i].clear();
    }

    shadowScroller.scrollUp(font.getCharHeight());
    shadowLfb.flush();
}

uint16_t LinearFrameBufferTerminal::getCurrentColumn() const {
    return currentColumn;
}

uint16_t LinearFrameBufferTerminal::getCurrentRow() const {
    return currentRow;
}

}