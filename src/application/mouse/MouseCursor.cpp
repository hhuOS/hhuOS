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

#include "MouseCursor.h"

#include "lib/util/game/Graphics2D.h"
#include "lib/util/memory/String.h"

void MouseCursor::draw(Util::Game::Graphics2D &graphics) const {
    auto string = Util::Memory::String();
    if (leftPressed) string += "l";
    if (middlePressed) string += "m";
    if (rightPressed) string += "r";

    graphics.drawString(posX, posY, string.isEmpty() ? "@" : string);
}

void MouseCursor::keyPressed(Key key) {
    switch (key) {
        case Util::Game::MouseListener::LEFT:
            leftPressed = true;
            break;
        case Util::Game::MouseListener::RIGHT:
            rightPressed = true;
            break;
        case Util::Game::MouseListener::MIDDLE:
            middlePressed = true;
            break;
        default:
            break;
    }
}

void MouseCursor::keyReleased(Key key) {
    switch (key) {
        case Util::Game::MouseListener::LEFT:
            leftPressed = false;
            break;
        case Util::Game::MouseListener::RIGHT:
            rightPressed = false;
            break;
        case Util::Game::MouseListener::MIDDLE:
            middlePressed = false;
            break;
        default:
            break;
    }
}

void MouseCursor::mouseMoved(double relativeX, double relativeY) {
    MouseCursor::posX += relativeX;
    MouseCursor::posY += relativeY;

    if (posX < -1) posX = -1;
    if (posX > 1) posX = 1;
    if (posY < -1) posY = -1;
    if (posY > 1) posY = 1;
}
