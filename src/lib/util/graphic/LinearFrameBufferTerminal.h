/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LINEARFRAMEBUFFERTERMINAL_H
#define HHUOS_LINEARFRAMEBUFFERTERMINAL_H

#include <stdint.h>

#include "font/Terminal8x16.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/Terminal.h"
#include "lib/util/graphic/Color.h"

namespace Util {
namespace Graphic {
class CursorRunnable;
class Font;
class LinearFrameBuffer;
}  // namespace Graphic
}  // namespace Util

namespace Util::Graphic {

class LinearFrameBufferTerminal : public Terminal {

public:

    friend class CursorRunnable;

    explicit LinearFrameBufferTerminal(LinearFrameBuffer *lfb, char cursor = static_cast<char>(219));

    LinearFrameBufferTerminal(const LinearFrameBufferTerminal &copy) = delete;

    LinearFrameBufferTerminal &operator=(const LinearFrameBufferTerminal &other) = delete;

    ~LinearFrameBufferTerminal() override;

    void putChar(char c, const Color &foregroundColor, const Color &backgroundColor) override;

    void clear(const Color &foregroundColor, const Color &backgroundColor, uint16_t startColumn, uint32_t startRow, uint16_t endColumn, uint16_t endRow) override;

    void setPosition(uint16_t column, uint16_t row) override;

    void setCursor(bool enabled) override;

    [[nodiscard]] uint16_t getCurrentColumn() const override;

    [[nodiscard]] uint16_t getCurrentRow() const override;

private:

    struct Character {
        char value;
        Color foregroundColor;
        Color backgroundColor;

        void clear() {
            value = 0;
            foregroundColor = Colors::WHITE;
            backgroundColor = Colors::BLACK;
        }
    };

    void scrollUp();

    Character *characterBuffer;

    LinearFrameBuffer &lfb;
    BufferedLinearFrameBuffer shadowLfb;

    const Font &font = Fonts::TERMINAL_8x16;
    uint16_t currentColumn = 0;
    uint16_t currentRow = 0;

    char cursor;
    CursorRunnable *cursorRunnable = nullptr;
    Async::Spinlock cursorLock;
};

}

#endif
