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

#include "MouseInputHandler.h"

#include "util/io/key/MouseDecoder.h"

MouseInputHandler::MouseInputHandler(const int32_t maxMouseX, const int32_t maxMouseY) : maxMouseX(maxMouseX), maxMouseY(maxMouseY) {
    mouseInputStream.setAccessMode(Util::Io::File::NON_BLOCKING);
}

bool MouseInputHandler::checkMouseInput() {
    auto mouseByte = mouseInputStream.read();
    while (mouseByte >= 0) {
        mouseInputBuffer[mouseInputIndex++] = static_cast<uint8_t>(mouseByte);

        if (mouseInputIndex < 4) {
            mouseByte = mouseInputStream.read();
        } else {
            break;
        }
    }

    if (mouseInputIndex == 4) {
        const auto mouseUpdate = Util::Io::MouseDecoder::decode(mouseInputBuffer);

        mousePositionChanged = mouseUpdate.xMovement != 0 || mouseUpdate.yMovement != 0;
        if (mouseUpdate.xMovement != 0 || mouseUpdate.yMovement != 0) {
            mouseX += mouseUpdate.xMovement;
            mouseY -= mouseUpdate.yMovement;

            if (mouseX < 0) {
                mouseX = 0;
            } else if (mouseX >= maxMouseX) {
                mouseX = maxMouseX - 1;
            }

            if (mouseY < 0) {
                mouseY = 0;
            } else if (mouseY >= maxMouseY) {
                mouseY = maxMouseY - 1;
            }
        }

        lastMouseButtonState = currentMouseButtonState;
        currentMouseButtonState = mouseUpdate.buttons;
        mouseInputIndex = 0;

        return true;
    }

    return false;
}
