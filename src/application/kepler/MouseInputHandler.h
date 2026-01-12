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

#ifndef HHUOS_MOUSEINPUTHANDLER_H
#define HHUOS_MOUSEINPUTHANDLER_H

#include <stddef.h>
#include <stdint.h>

#include "util/io/key/MouseDecoder.h"
#include "util/io/stream/FileInputStream.h"

class MouseInputHandler {

public:

    MouseInputHandler(int32_t maxMouseX, int32_t maxMouseY);

    ~MouseInputHandler() = default;

    bool checkMouseInput();

    int32_t getMousePosX() const {
        return mouseX;
    }

    int32_t getMousePosY() const {
        return mouseY;
    }

    bool hasMousePositionChanged() const {
        return mousePositionChanged;
    }

    bool isButtonCurrentlyPressed(const Util::Io::MouseDecoder::Button button) const {
        return currentMouseButtonState & button;
    }

    bool wasButtonPressed(const Util::Io::MouseDecoder::Button button) const {
        return currentMouseButtonState & button && !(lastMouseButtonState & button);
    }

    bool wasButtonReleased(const Util::Io::MouseDecoder::Button button) const {
        return !(currentMouseButtonState & button) && lastMouseButtonState & button;
    }

private:

    Util::Io::FileInputStream mouseInputStream = Util::Io::FileInputStream("/device/mouse");

    size_t mouseInputIndex = 0;
    uint8_t mouseInputBuffer[4];

    int32_t mouseX = 0;
    int32_t mouseY = 0;
    int32_t maxMouseX = 0;
    int32_t maxMouseY = 0;
    bool mousePositionChanged = false;

    uint8_t currentMouseButtonState = 0;
    uint8_t lastMouseButtonState = 0;
};

#endif