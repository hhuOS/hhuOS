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

#include "LinearFrameBufferTerminal.h"
#include "lib/util/async/Thread.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/graphic/Font.h"
#include "lib/util/time/Timestamp.h"

namespace Util::Graphic {

CursorRunnable::CursorRunnable(LinearFrameBufferTerminal &terminal, char cursor) : terminal(terminal), cursor(cursor) {}

void CursorRunnable::run() {
    while (isRunning) {
        terminal.cursorLock.acquire();
        draw();
        visible = !visible;
        terminal.cursorLock.release();

        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(250));
    }

    visible = false;
    draw();
}

void CursorRunnable::stop() {
    isRunning = false;
}

void CursorRunnable::draw() {
    const auto character = terminal.characterBuffer[terminal.currentRow * terminal.getColumns() + terminal.currentColumn];
    terminal.lfb.drawChar(terminal.font,
        terminal.currentColumn * terminal.font.getCharWidth(),
        terminal.currentRow * terminal.font.getCharHeight(),
        visible ? cursor : character.value,
        character.foregroundColor,
        character.backgroundColor);
}

}