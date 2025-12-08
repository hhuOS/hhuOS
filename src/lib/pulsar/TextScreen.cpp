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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#include <stddef.h>

#include "TextScreen.h"

namespace Pulsar {

bool TextScreen::initializeBackground(Graphics &graphics) {
    const auto lines = text.split("\n");
    const auto centerX = graphics.getAbsoluteResolutionX() / 2;
    const auto centerY = graphics.getAbsoluteResolutionY() / 2;
    const auto y = static_cast<uint16_t>(centerY - lines.length() * Graphics::FONT_SIZE / 2.0);

    graphics.clear(backgroundColor);
    graphics.setColor(fontColor);

    for (size_t i = 0; i < lines.length(); ++i) {
        const auto &line = lines[i];
        const auto x = static_cast<uint16_t>(centerX - line.length() * Graphics::FONT_SIZE / 2.0);
        graphics.drawStringDirectAbsolute(x, y + i * Graphics::FONT_SIZE, line);
    }

    return true;
}

}
