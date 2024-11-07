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

#include "CursorRunnable.h"
#include "lib/util/base/Address.h"
#include "LinearFrameBufferTerminal.h"
#include "lib/util/graphic/Font.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/async/Thread.h"
#include "lib/util/base/Exception.h"

namespace Util::Graphic {

LinearFrameBufferTerminal::LinearFrameBufferTerminal(Util::Graphic::LinearFrameBuffer *lfb, char cursor) :
        Terminal(lfb->getResolutionX() / Font::getFontForResolution(lfb->getResolutionY()).getCharWidth(), lfb->getResolutionY() / Font::getFontForResolution(lfb->getResolutionY()).getCharHeight()),
        characterBuffer(new Character[getColumns() * getRows()]), lfb(*lfb), pixelDrawer(*lfb), stringDrawer(pixelDrawer), shadowLfb(*lfb),
        shadowPixelDrawer(shadowLfb), shadowStringDrawer(shadowPixelDrawer), shadowScroller(shadowLfb), font(Font::getFontForResolution(lfb->getResolutionY())), cursor(cursor) {
    Terminal::clear();
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
        Util::Async::Thread::yield();
    }

    characterBuffer[currentRow * getColumns() + currentColumn] = {c, foregroundColor, backgroundColor};

    stringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), c, foregroundColor, backgroundColor);
    shadowStringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), c, foregroundColor, backgroundColor);
    currentColumn++;

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

void LinearFrameBufferTerminal::clear(const Util::Graphic::Color &foregroundColor, const Util::Graphic::Color &backgroundColor, uint16_t startColumn, uint32_t startRow, uint16_t endColumn, uint16_t endRow) {
    while (!cursorLock.tryAcquire()) {
        cursorLock.release();
        Util::Async::Thread::yield();
    }

    if (startRow == endRow) {
        // Clear from start column to end column
        for (uint32_t i = 0; i < static_cast<uint32_t>(endColumn - startColumn + 1) * font.getCharWidth(); i++) {
            for (uint32_t j = 0; j < font.getCharHeight(); j++) {
                shadowPixelDrawer.drawPixel((startColumn * font.getCharWidth()) + i, (startRow * font.getCharHeight()) + j, backgroundColor);
            }
        }
    } else if (startRow < endRow) {
        // Clear from start position to end of line
        for (uint32_t y = 0; y < font.getCharHeight(); y++) {
            for (uint32_t x = 0; x < static_cast<uint32_t>(getColumns() - startColumn) * font.getCharWidth(); x++) {
                shadowPixelDrawer.drawPixel((startColumn * font.getCharWidth()) + x, (startRow * font.getCharHeight()) + y, backgroundColor);
            }
        }

        // Clear from next line to before last line
        for (uint32_t y = 0; y < (endRow - startRow - 1) * font.getCharHeight(); y++) {
            for (uint32_t x = 0; x < getColumns() * font.getCharWidth(); x++) {
                shadowPixelDrawer.drawPixel(x, ((startRow + 1) * font.getCharHeight()) + y, backgroundColor);
            }
        }

        // Clear from beginning of last line to end position
        for (uint32_t y = 0; y < font.getCharHeight(); y++) {
            for (uint32_t x = 0; x < endColumn * font.getCharWidth(); x++) {
                shadowPixelDrawer.drawPixel(x, (endRow * font.getCharHeight()) + y, backgroundColor);
            }
        }
    } else {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Terminal: Invalid arguments for clear()!");
    }

    for (uint32_t i = startRow * getColumns() + startColumn; i < static_cast<uint32_t>(endRow * getColumns()) + endColumn + 1; i++) {
        characterBuffer[i] = {'\0', foregroundColor, backgroundColor};
    }

    shadowLfb.flush();
    cursorLock.release();
}

void LinearFrameBufferTerminal::setPosition(uint16_t column, uint16_t row) {
    while (!cursorLock.tryAcquire()) {
        cursorLock.release();
        Util::Async::Thread::yield();
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
    // Scroll character buffer
    auto characterAddress = Util::Address<uint32_t>(characterBuffer);
    characterAddress.copyRange(characterAddress.add(getColumns() * sizeof(Character)), getColumns() * (getRows() - 1) * sizeof(Character));

    // Scroll shadow LFB
    shadowScroller.scrollUp(font.getCharHeight(), false);

    // Clear last line (Also flushes shadow buffer)
    clear(getForegroundColor(), getBackgroundColor(), 0, getRows() - 1, getColumns() - 1, getRows() - 1);
}

uint16_t LinearFrameBufferTerminal::getCurrentColumn() const {
    return currentColumn;
}

uint16_t LinearFrameBufferTerminal::getCurrentRow() const {
    return currentRow;
}

}