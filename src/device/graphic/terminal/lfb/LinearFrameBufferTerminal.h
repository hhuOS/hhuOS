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

#ifndef HHUOS_LINEARFRAMEBUFFERTERMINAL_H
#define HHUOS_LINEARFRAMEBUFFERTERMINAL_H

#include <lib/util/graphic/LinearFrameBuffer.h>
#include <lib/util/graphic/Colors.h>
#include <lib/util/graphic/PixelDrawer.h>
#include <lib/util/graphic/StringDrawer.h>
#include <lib/util/graphic/Fonts.h>
#include <lib/util/graphic/BufferScroller.h>
#include <lib/util/async/Spinlock.h>
#include <device/graphic/terminal/Terminal.h>
#include "CursorRunnable.h"

namespace Device::Graphic {

class LinearFrameBufferTerminal : public Terminal {

public:

    friend class CursorRunnable;

    explicit LinearFrameBufferTerminal(Util::Graphic::LinearFrameBuffer *lfb, Util::Graphic::Font &font = Util::Graphic::Fonts::TERMINAL_FONT);

    LinearFrameBufferTerminal(const LinearFrameBufferTerminal &copy) = delete;

    LinearFrameBufferTerminal &operator=(const LinearFrameBufferTerminal &other) = delete;

    ~LinearFrameBufferTerminal() override;

    void putChar(char c, const Util::Graphic::Color &foregroundColor, const Util::Graphic::Color &backgroundColor) override;

    void setPosition(uint16_t column, uint16_t row) override;

    void clear(const Util::Graphic::Color &backgroundColor) override;

private:

    Util::Graphic::LinearFrameBuffer &lfb;
    Util::Graphic::BufferScroller scroller;
    Util::Graphic::PixelDrawer pixelDrawer;
    Util::Graphic::StringDrawer stringDrawer;

    Util::Graphic::Font &font;
    uint16_t currentColumn = 0;
    uint16_t currentRow = 0;

    CursorRunnable cursorRunnable;
    Util::Async::Spinlock cursorLock;
};

}

#endif
