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
#include <kernel/service/JobService.h>
#include <kernel/system/System.h>
#include "LinearFrameBufferTerminal.h"

namespace Device::Graphic {

LinearFrameBufferTerminal::LinearFrameBufferTerminal(Util::Graphic::LinearFrameBuffer *lfb, Util::Graphic::Font &font, char cursor) :
        Terminal(lfb->getResolutionX() / font.getCharWidth(), lfb->getResolutionY() / font.getCharHeight()),
        characterBuffer(new Character[getColumns() * getRows()]), lfb(*lfb), pixelDrawer(*lfb), stringDrawer(pixelDrawer), shadowLfb(*lfb),
        shadowPixelDrawer(shadowLfb), shadowStringDrawer(shadowPixelDrawer), shadowScroller(shadowLfb), font(font) {
    LinearFrameBufferTerminal::clear(Util::Graphic::Colors::BLACK);
    Kernel::System::getService<Kernel::JobService>().registerJob(new CursorRunnable(*this, cursor), Kernel::Job::Priority::LOW, Util::Time::Timestamp(0, 250000000));
}

LinearFrameBufferTerminal::~LinearFrameBufferTerminal() {
    delete &lfb;
    delete[] characterBuffer;
}

void LinearFrameBufferTerminal::putChar(char c, const Util::Graphic::Color &foregroundColor, const Util::Graphic::Color &backgroundColor) {
    cursorLock.acquire();

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
    cursorLock.acquire();

    for (uint32_t i = 0; i < getColumns() * getRows(); i++) {
        characterBuffer[i].clear();
    }

    lfb.clear();
    currentRow = 0;
    currentColumn = 0;

    cursorLock.release();
}

void LinearFrameBufferTerminal::setPosition(uint16_t column, uint16_t row) {
    cursorLock.acquire();

    currentColumn = column;
    currentRow = row;

    cursorLock.release();
}

void LinearFrameBufferTerminal::scrollUp() {
    auto characterAddress = Util::Memory::Address<uint32_t>(characterBuffer, getColumns() * getRows() * sizeof(Character));
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