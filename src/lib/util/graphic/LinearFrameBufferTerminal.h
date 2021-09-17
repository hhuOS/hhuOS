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

#include "LinearFrameBuffer.h"
#include "Colors.h"
#include "PixelDrawer.h"
#include "StringDrawer.h"
#include "Fonts.h"
#include "BufferScroller.h"
#include "Terminal.h"

namespace Util::Graphic {

class LinearFrameBufferTerminal : public Terminal {

public:

    explicit LinearFrameBufferTerminal(LinearFrameBuffer &lfb, Font &font = Fonts::TERMINAL_FONT);

    LinearFrameBufferTerminal(const LinearFrameBufferTerminal &copy) = delete;

    LinearFrameBufferTerminal &operator=(const LinearFrameBufferTerminal &other) = delete;

    ~LinearFrameBufferTerminal() override = default;

    void putChar(char c, const Color &foregroundColor, const Color &backgroundColor) override;

    void setPosition(uint16_t column, uint16_t row) override;

    void clear(const Color &backgroundColor) override;

    [[nodiscard]] LinearFrameBuffer& getLinearFrameBuffer() const;

private:

    void updateCursorPosition();

    LinearFrameBuffer &lfb;
    BufferScroller scroller;
    PixelDrawer pixelDrawer;
    StringDrawer stringDrawer;

    Font &font;
    uint16_t currentColumn = 0;
    uint16_t currentRow = 0;
};

}

#endif
