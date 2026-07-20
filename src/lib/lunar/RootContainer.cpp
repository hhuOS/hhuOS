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
 *
 * The widget and layout system is based on a bachelor's thesis, written by Michael Zuchniewski.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-mizuc100
 */

#include "RootContainer.h"

namespace Lunar {

void RootContainer::onMouseHover(const uint16_t x, const uint16_t y) {
    auto *hoveredChild = getChildAtPoint(x, y);

    if (hoveredChild != lastHoveredChild) {
        if (lastHoveredChild != nullptr) {
            lastHoveredChild->mouseExited();
        }
        if (hoveredChild != nullptr) {
            hoveredChild->mouseEntered();
        }
    }

    lastHoveredChild = hoveredChild;
}

void RootContainer::onMouseClick(const uint16_t x, const uint16_t y, const Util::Io::MouseDecoder::Button button,
    const bool pressed)
{
    if (button == Util::Io::MouseDecoder::LEFT_BUTTON) {
        if (pressed) {
            auto *clickedChild = getChildAtPoint(x, y);
            if (clickedChild != lastPressedChild && lastPressedChild != nullptr) {
                lastPressedChild->setFocused(false);
            }

            if (clickedChild != nullptr) {
                clickedChild->setFocused(true);
                clickedChild->mousePressed();
            }

            lastPressedChild = clickedChild;
        } else {
            if (lastPressedChild != nullptr) {
                lastPressedChild->mouseReleased();
                lastPressedChild->mouseClicked();
            }
        }
    }
}

void RootContainer::onKeyEvent(const Util::Io::KeyEvent &keyEvent) const {
    if (lastPressedChild != nullptr) {
        if (keyEvent.isPressed()) {
            lastPressedChild->keyPressed(keyEvent);
        } else {
            lastPressedChild->keyReleased(keyEvent);
            lastPressedChild->keyTyped(keyEvent);
        }
    }
}

}
