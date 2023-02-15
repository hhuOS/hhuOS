/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "DinoGame.h"

void DinoGame::update(double delta) {
    if (getObjectCount() == 0) {
        dino = new Dino();
        setKeyListener(*this);
        addObject(dino);
    } else {
        dino->update(delta);
    }
}

void DinoGame::keyPressed(Util::Io::Key key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC :
            stop();
            break;
        case Util::Io::Key::F1 :
            dino->die();
            break;
        case Util::Io::Key::SPACE :
            dino->hatch();
            break;
        case Util::Io::Key::LEFT :
            dino->moveLeft();
            dino->dash(key.getShift());
            leftPressed = true;
            break;
        case Util::Io::Key::RIGHT :
            dino->moveRight();
            dino->dash(key.getShift());
            rightPressed = true;
    }
}

void DinoGame::keyReleased(Util::Io::Key key) {
    switch (key.getScancode()) {
        case Util::Io::Key::LEFT:
            leftPressed = false;
            break;
        case Util::Io::Key::RIGHT:
            rightPressed = false;
            break;
    }

    if (!leftPressed && !rightPressed) {
        dino->stop();
    }
}