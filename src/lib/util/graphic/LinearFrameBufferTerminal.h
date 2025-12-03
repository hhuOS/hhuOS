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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_LINEARFRAMEBUFFERTERMINAL_H
#define HHUOS_LIB_UTIL_GRAPHIC_LINEARFRAMEBUFFERTERMINAL_H

#include <stdint.h>

#include "util/async/Spinlock.h"
#include "util/graphic/BufferedLinearFrameBuffer.h"
#include "util/graphic/Color.h"
#include "util/graphic/Colors.h"
#include "util/graphic/Terminal.h"
#include "util/graphic/font/Terminal8x16.h"

namespace Util {
namespace Graphic {

/// A terminal implementation that uses a linear frame buffer for output.
class LinearFrameBufferTerminal final : public Terminal {

public:
    /// Create a new linear frame buffer terminal instance.
    explicit LinearFrameBufferTerminal(const LinearFrameBuffer &lfb, char cursor = static_cast<char>(219));

    /// Destroy the linear frame buffer terminal instance.
    ~LinearFrameBufferTerminal() override;

    /// Draw a character at the current cursor position.
    void putChar(char c, const Color &foregroundColor, const Color &backgroundColor) override;

    /// Clear a rectangular area of the terminal with the specified colors.
    void clear(const Color &foregroundColor, const Color &backgroundColor, uint16_t startColumn, uint16_t startRow,
        uint16_t endColumn, uint16_t endRow) override;

    /// Set the cursor position to the specified column and row.
    void setPosition(uint16_t column, uint16_t row) override;

    /// Enable or disable the terminal cursor.
    void setCursorEnabled(bool enabled) override;

    /// Get the current cursor column.
    uint16_t getCurrentColumn() const override {
        return currentColumn;
    }

    /// Get the current cursor row.
    uint16_t getCurrentRow() const override {
        return currentRow;
    }

private:

    class CursorRunnable final : public Async::Runnable {

    public:

        explicit CursorRunnable(LinearFrameBufferTerminal &terminal, const char cursor) :
            terminal(terminal), cursor(cursor) {}

        void run() override;

        void stop();

        void draw() const;

    private:

        LinearFrameBufferTerminal &terminal;
        char cursor;

        bool visible = true;
        bool isRunning = true;
    };

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

    const LinearFrameBuffer &lfb;
    BufferedLinearFrameBuffer shadowLfb;

    const Font &font = Fonts::TERMINAL_8x16;
    uint16_t currentColumn = 0;
    uint16_t currentRow = 0;

    char cursor;
    CursorRunnable *cursorRunnable = nullptr;
    Async::Spinlock cursorLock;
};

}
}

#endif
