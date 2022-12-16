/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <cstdint>

#include "lib/util/graphic/Colors.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/BufferScroller.h"
#include "lib/util/graphic/Terminal.h"
#include "lib/util/graphic/Color.h"

namespace Device {
namespace Graphic {
class CursorRunnable;
}  // namespace Graphic
}  // namespace Device
namespace Util {
namespace Graphic {
class Font;
class LinearFrameBuffer;
}  // namespace Graphic
}  // namespace Util

namespace Device::Graphic {

class LinearFrameBufferTerminal : public Util::Graphic::Terminal {

public:

    friend class CursorRunnable;

    explicit LinearFrameBufferTerminal(Util::Graphic::LinearFrameBuffer *lfb, Util::Graphic::Font &font, char cursor);

    LinearFrameBufferTerminal(const LinearFrameBufferTerminal &copy) = delete;

    LinearFrameBufferTerminal &operator=(const LinearFrameBufferTerminal &other) = delete;

    ~LinearFrameBufferTerminal() override;

    void putChar(char c, const Util::Graphic::Color &foregroundColor, const Util::Graphic::Color &backgroundColor) override;

    void clear(const Util::Graphic::Color &backgroundColor) override;

    void setPosition(uint16_t column, uint16_t row) override;

    void setCursor(bool enabled) override;

    [[nodiscard]] uint16_t getCurrentColumn() const override;

    [[nodiscard]] uint16_t getCurrentRow() const override;

private:

    struct Character {
        char value;
        Util::Graphic::Color foregroundColor;
        Util::Graphic::Color backgroundColor;

        void clear() {
            value = 0;
            foregroundColor = Util::Graphic::Colors::WHITE;
            backgroundColor = Util::Graphic::Colors::BLACK;
        }
    };

    void scrollUp();

    Character *characterBuffer;

    Util::Graphic::LinearFrameBuffer &lfb;
    Util::Graphic::PixelDrawer pixelDrawer;
    Util::Graphic::StringDrawer stringDrawer;

    Util::Graphic::BufferedLinearFrameBuffer shadowLfb;
    Util::Graphic::PixelDrawer shadowPixelDrawer;
    Util::Graphic::StringDrawer shadowStringDrawer;
    Util::Graphic::BufferScroller shadowScroller;

    Util::Graphic::Font &font;
    uint16_t currentColumn = 0;
    uint16_t currentRow = 0;

    char cursor;
    CursorRunnable *cursorRunnable = nullptr;
    Util::Async::Spinlock cursorLock;
};

}

#endif
