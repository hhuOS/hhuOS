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

#include "CubeDemo.h"
#include "lib/util/stream/PrintWriter.h"

CubeDemo::CubeDemo(uint32_t speed) : speed(speed) {
    for (auto *cube : cubes) {
        addObject(cube);
    }

    setKeyListener(*this);
}

void CubeDemo::update(double delta) {
    for (auto &cube : cubes) {
        cube->rotate(ANGLE_X * speed * delta, ANGLE_Y * speed * delta, ANGLE_Z * speed * delta);
    }
}

void CubeDemo::keyPressed(char c) {switch (c) {
        case '+': {
            speed++;
            break;
        }
        case '-': {
            speed--;
            break;
        }
        default:
            stop();
    }
}
