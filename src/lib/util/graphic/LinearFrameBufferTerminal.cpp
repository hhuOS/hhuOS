/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "LinearFrameBufferTerminal.h"

#include "util/async/Thread.h"
#include "util/base/Address.h"
#include "util/base/Panic.h"
#include "util/graphic/Font.h"
#include "util/graphic/LinearFrameBuffer.h"
#include "util/time/Timestamp.h"

namespace Util {
namespace Graphic {

LinearFrameBufferTerminal::LinearFrameBufferTerminal(const LinearFrameBuffer &lfb, const char cursor) :
    Terminal(lfb.getResolutionX() / Fonts::TERMINAL_8x16.getCharWidth(),
             lfb.getResolutionY() / Fonts::TERMINAL_8x16.getCharHeight()),
    characterBuffer(new Character[getColumns() * getRows()]), lfb(lfb), shadowLfb(lfb), cursor(cursor)
{
    Terminal::clear();
    setCursorEnabled(true);
}

LinearFrameBufferTerminal::~LinearFrameBufferTerminal() {
    setCursorEnabled(false);
    delete[] characterBuffer;
}

void LinearFrameBufferTerminal::putChar(const char c, const Color &foregroundColor, const Color &backgroundColor) {
    while (!cursorLock.tryAcquire()) {
        cursorLock.release();
        Async::Thread::yield();
    }

    characterBuffer[currentRow * getColumns() + currentColumn] = Character{c, foregroundColor, backgroundColor};

    lfb.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), c,
                 foregroundColor, backgroundColor);
    shadowLfb.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), c,
                       foregroundColor, backgroundColor);

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

void LinearFrameBufferTerminal::clear(const Color &foregroundColor, const Color &backgroundColor,
    const uint16_t startColumn, const uint16_t startRow, const uint16_t endColumn, const uint16_t endRow)
{
    while (!cursorLock.tryAcquire()) {
        cursorLock.release();
        Async::Thread::yield();
    }

    if (startRow == endRow) {
        // Clear from start column to end column
        for (int32_t i = 0; i < (endColumn - startColumn + 1) * font.getCharWidth(); i++) {
            for (int32_t j = 0; j < font.getCharHeight(); j++) {
                shadowLfb.drawPixel(startColumn * font.getCharWidth() + i,
                    startRow * font.getCharHeight() + j, backgroundColor);
            }
        }
    } else if (startRow < endRow) {
        // Clear from start position to end of line
        for (int32_t y = 0; y < font.getCharHeight(); y++) {
            for (int32_t x = 0; x < getColumns() - startColumn * font.getCharWidth(); x++) {
                shadowLfb.drawPixel(startColumn * font.getCharWidth() + x,
                    startRow * font.getCharHeight() + y, backgroundColor);
            }
        }

        // Clear from next line to before last line
        for (int32_t y = 0; y < (endRow - startRow - 1) * font.getCharHeight(); y++) {
            for (int32_t x = 0; x < getColumns() * font.getCharWidth(); x++) {
                shadowLfb.drawPixel(x, (startRow + 1) * font.getCharHeight() + y, backgroundColor);
            }
        }

        // Clear from beginning of last line to end position
        for (int32_t y = 0; y < font.getCharHeight(); y++) {
            for (int32_t x = 0; x < endColumn * font.getCharWidth(); x++) {
                shadowLfb.drawPixel(x, endRow * font.getCharHeight() + y, backgroundColor);
            }
        }
    } else {
        Panic::fire(Panic::INVALID_ARGUMENT, "Terminal: Invalid arguments for clear()!");
    }

    for (int32_t i = startRow * getColumns() + startColumn; i < endRow * getColumns() + endColumn + 1; i++) {
        characterBuffer[i] = Character{'\0', foregroundColor, backgroundColor};
    }

    shadowLfb.flush();
    cursorLock.release();
}

void LinearFrameBufferTerminal::setPosition(const uint16_t column, const uint16_t row) {
    while (!cursorLock.tryAcquire()) {
        cursorLock.release();
        Async::Thread::yield();
    }

    const auto character = characterBuffer[currentRow * getColumns() + currentColumn];
    lfb.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), character.value,
        character.foregroundColor, character.backgroundColor);

    currentColumn = column;
    currentRow = row;

    while (currentRow >= getRows()) {
        scrollUp();
        currentRow--;
    }

    cursorLock.release();
}

void LinearFrameBufferTerminal::setCursorEnabled(const bool enabled) {
    cursorLock.acquire();

    if (enabled) {
        shadowLfb.flush();

        if (cursorRunnable != nullptr) {
            cursorLock.release();
            return;
        }

        cursorRunnable = new CursorRunnable(*this, cursor);
        Async::Thread::createThread("Cursor", cursorRunnable);
    } else if (cursorRunnable != nullptr) {
        cursorRunnable->stop();
        cursorRunnable = nullptr;
    }

    cursorLock.release();
}

void LinearFrameBufferTerminal::CursorRunnable::run() {
    while (isRunning) {
        terminal.cursorLock.acquire();
        draw();
        visible = !visible;
        terminal.cursorLock.release();

        Async::Thread::sleep(Time::Timestamp::ofMilliseconds(250));
    }

    visible = false;
    draw();
}

void LinearFrameBufferTerminal::CursorRunnable::stop() {
    isRunning = false;
}

void LinearFrameBufferTerminal::CursorRunnable::draw() const {
    const auto character = terminal.characterBuffer[
        terminal.currentRow * terminal.getColumns() + terminal.currentColumn];

    terminal.lfb.drawChar(terminal.font,
        terminal.currentColumn * terminal.font.getCharWidth(),
        terminal.currentRow * terminal.font.getCharHeight(),
        visible ? cursor : character.value,
        character.foregroundColor,
        character.backgroundColor);
}

void LinearFrameBufferTerminal::scrollUp() {
    // Scroll character buffer
    const auto characterAddress = Address(characterBuffer);
    characterAddress.copyRange(characterAddress.add(getColumns() * sizeof(Character)),
        getColumns() * (getRows() - 1) * sizeof(Character));

    // Scroll shadow LFB
    shadowLfb.scrollUp(font.getCharHeight(), false);

    // Clear last line (Also flushes shadow buffer)
    clear(getForegroundColor(), getBackgroundColor(), 0, getRows() - 1,
        getColumns() - 1, getRows() - 1);
}

}
}
