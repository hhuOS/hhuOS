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

#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/LineDrawer.h"

Util::Graphic::LinearFrameBuffer lfb(Util::File::File("/device/lfb"));
Util::Graphic::BufferedLinearFrameBuffer bufferedLfb(lfb);
Util::Graphic::PixelDrawer pixelDrawer(bufferedLfb);
Util::Graphic::LineDrawer lineDrawer(pixelDrawer);

void transformedDrawLine(double x1, double y1, double x2, double y2, const Util::Graphic::Color &color) {
    int32_t TRANSLATION_X = lfb.getResolutionX() / 2;
    int32_t TRANSLATION_Y = lfb.getResolutionY() / 2;
    lineDrawer.drawLine(static_cast<uint16_t>(x1 + TRANSLATION_X), static_cast<uint16_t>(y1 + TRANSLATION_Y), static_cast<uint16_t>(x2 + TRANSLATION_X), static_cast<uint16_t>(y2 + TRANSLATION_Y), color);
}

int32_t main(int32_t argc, char *argv[]) {
    double cube[8][4];
    uint32_t x=1, y=2, z=3;
    auto color = Util::Graphic::Color(10,255,0);

    // 8 corner points in the local cube coordinate system
    // Middle of the cube is (0,0)
    cube[0][x] = -100; cube[0][y] = -100; cube[0][z] = -100;
    cube[1][x] = +100; cube[1][y] = -100; cube[1][z] = -100;
    cube[2][x] = +100; cube[2][y] = -100; cube[2][z] = +100;
    cube[3][x] = -100; cube[3][y] = -100; cube[3][z] = +100;
    cube[4][x] = -100; cube[4][y] = +100; cube[4][z] = -100;
    cube[5][x] = +100; cube[5][y] = +100; cube[5][z] = -100;
    cube[6][x] = +100; cube[6][y] = +100; cube[6][z] = +100;
    cube[7][x] = -100; cube[7][y] = +100; cube[7][z] = +100;

    //       7 - - - - - 6
    //     / |         / |
    //    4 - - - - - 5  |
    //    |  |        |  |
    //    |  3 - - - -|- 2
    //    | /         | /
    //    0 - - - - - 1

    double sin_angle_x=0.000999, cos_angle_x=0.999999;
    double sin_angle_y=0.006999, cos_angle_y=0.999975;
    double sin_angle_z=0.004999, cos_angle_z=0.999987;

    // Animation loop
    while (true) {
        double px, py, pz;

        // Rotate cube and recalculate corner points
        for (auto & corner : cube) {
            px = corner[x];
            py = corner[y];
            pz = corner[z];

            // Rotate around x-axis
            corner[y] = py * cos_angle_x - pz * sin_angle_x;
            corner[z] = py * sin_angle_x + pz * cos_angle_x;

            py = corner[y];
            pz = corner[z];

            // Rotate around y-axis
            corner[x] = px * cos_angle_y + pz * sin_angle_y;
            corner[z] = -px * sin_angle_y + pz * cos_angle_y;

            px = corner[x];

            // Rotate around z-axis
            corner[x] = px * cos_angle_z - py * sin_angle_z;
            corner[y] = py * cos_angle_z + px * sin_angle_z;
        }

        bufferedLfb.clear();

        // Draw cube
        transformedDrawLine( cube[0][x], cube[0][y], cube[1][x], cube[1][y], color);
        transformedDrawLine( cube[1][x], cube[1][y], cube[2][x], cube[2][y], color);
        transformedDrawLine( cube[2][x], cube[2][y], cube[3][x], cube[3][y], color);
        transformedDrawLine( cube[3][x], cube[3][y], cube[0][x], cube[0][y], color);
        transformedDrawLine( cube[4][x], cube[4][y], cube[5][x], cube[5][y], color);
        transformedDrawLine( cube[5][x], cube[5][y], cube[6][x], cube[6][y], color);
        transformedDrawLine( cube[6][x], cube[6][y], cube[7][x], cube[7][y], color);
        transformedDrawLine( cube[7][x], cube[7][y], cube[4][x], cube[4][y], color);
        transformedDrawLine( cube[0][x], cube[0][y], cube[4][x], cube[4][y], color);
        transformedDrawLine( cube[1][x], cube[1][y], cube[5][x], cube[5][y], color);
        transformedDrawLine( cube[2][x], cube[2][y], cube[6][x], cube[6][y], color);
        transformedDrawLine( cube[3][x], cube[3][y], cube[7][x], cube[7][y], color);

        bufferedLfb.flush();
    }
}