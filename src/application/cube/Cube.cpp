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

#include "Cube.h"
#include "lib/util/math/Math.h"
#include "lib/util/graphic/LineDrawer.h"

const Util::Graphic::Color Cube::color = Util::Graphic::Color(10,255,0);

Cube::Cube() {
    // 8 corner points in the local coordinates coordinate system
    // Middle of the coordinates is (0,0)
    //
    //       7 - - - - - 6
    //     / |         / |
    //    4 - - - - - 5  |
    //    |  |        |  |
    //    |  3 - - - -|- 2
    //    | /         | /
    //    0 - - - - - 1
    coordinates[0][x] = -100; coordinates[0][y] = -100; coordinates[0][z] = -100;
    coordinates[1][x] = +100; coordinates[1][y] = -100; coordinates[1][z] = -100;
    coordinates[2][x] = +100; coordinates[2][y] = -100; coordinates[2][z] = +100;
    coordinates[3][x] = -100; coordinates[3][y] = -100; coordinates[3][z] = +100;
    coordinates[4][x] = -100; coordinates[4][y] = +100; coordinates[4][z] = -100;
    coordinates[5][x] = +100; coordinates[5][y] = +100; coordinates[5][z] = -100;
    coordinates[6][x] = +100; coordinates[6][y] = +100; coordinates[6][z] = +100;
    coordinates[7][x] = -100; coordinates[7][y] = +100; coordinates[7][z] = +100;
}

void Cube::draw(const Util::Graphic::LinearFrameBuffer &lfb) const {
    auto translationX = lfb.getResolutionX() / 2;
    auto translationY = lfb.getResolutionY() / 2;
    auto lineDrawer = Util::Graphic::LineDrawer(Util::Graphic::PixelDrawer(lfb));
    
    lineDrawer.drawLine(coordinates[0][x] + translationX, coordinates[0][y] + translationY, coordinates[1][x] + translationX, coordinates[1][y] + translationY, color);
    lineDrawer.drawLine(coordinates[1][x] + translationX, coordinates[1][y] + translationY, coordinates[2][x] + translationX, coordinates[2][y] + translationY, color);
    lineDrawer.drawLine(coordinates[2][x] + translationX, coordinates[2][y] + translationY, coordinates[3][x] + translationX, coordinates[3][y] + translationY, color);
    lineDrawer.drawLine(coordinates[3][x] + translationX, coordinates[3][y] + translationY, coordinates[0][x] + translationX, coordinates[0][y] + translationY, color);
    lineDrawer.drawLine(coordinates[4][x] + translationX, coordinates[4][y] + translationY, coordinates[5][x] + translationX, coordinates[5][y] + translationY, color);
    lineDrawer.drawLine(coordinates[5][x] + translationX, coordinates[5][y] + translationY, coordinates[6][x] + translationX, coordinates[6][y] + translationY, color);
    lineDrawer.drawLine(coordinates[6][x] + translationX, coordinates[6][y] + translationY, coordinates[7][x] + translationX, coordinates[7][y] + translationY, color);
    lineDrawer.drawLine(coordinates[7][x] + translationX, coordinates[7][y] + translationY, coordinates[4][x] + translationX, coordinates[4][y] + translationY, color);
    lineDrawer.drawLine(coordinates[0][x] + translationX, coordinates[0][y] + translationY, coordinates[4][x] + translationX, coordinates[4][y] + translationY, color);
    lineDrawer.drawLine(coordinates[1][x] + translationX, coordinates[1][y] + translationY, coordinates[5][x] + translationX, coordinates[5][y] + translationY, color);
    lineDrawer.drawLine(coordinates[2][x] + translationX, coordinates[2][y] + translationY, coordinates[6][x] + translationX, coordinates[6][y] + translationY, color);
    lineDrawer.drawLine(coordinates[3][x] + translationX, coordinates[3][y] + translationY, coordinates[7][x] + translationX, coordinates[7][y] + translationY, color);
}

void Cube::rotate(double angleX, double angleY, double angleZ) {
    double px, py, pz;

    // Rotate coordinates and recalculate corner points
    for (auto &corner : coordinates) {
        px = corner[x];
        py = corner[y];
        pz = corner[z];

        // Rotate around x-axis
        corner[y] = py * Util::Math::Math::cosine(angleX) - pz * Util::Math::Math::sine(angleX);
        corner[z] = py * Util::Math::Math::sine(angleX) + pz * Util::Math::Math::cosine(angleX);

        py = corner[y];
        pz = corner[z];

        // Rotate around y-axis
        corner[x] = px * Util::Math::Math::cosine(angleY) + pz * Util::Math::Math::sine(angleY);
        corner[z] = -px * Util::Math::Math::sine(angleY) + pz * Util::Math::Math::cosine(angleY);

        px = corner[x];

        // Rotate around z-axis
        corner[x] = px * Util::Math::Math::cosine(angleZ) - py * Util::Math::Math::sine(angleZ);
        corner[y] = py * Util::Math::Math::cosine(angleZ) + px * Util::Math::Math::sine(angleZ);
    }
}
