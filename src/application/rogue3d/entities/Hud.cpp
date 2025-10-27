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
 * The rogue game has been implemented during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#include "lib/util/base/String.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/math/Vector2.h"
#include "Hud.h"
#include "Player.h"
#include "Room.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/math/Vector3.h"

Hud::Hud(Room::Type map[4][4], const Player &player) : Entity(TAG, Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0, 0, 0)), player(player) {
    for (uint32_t row = 0; row < 4; row++) {
        for (uint32_t column = 0; column < 4; column++) {
           Hud::map[row][column] = map[row][column];
        }
    }
}

void Hud::initialize() {}

void Hud::draw(Util::Game::Graphics &graphics) const {
    const auto charSize = Util::Game::Graphics::FONT_SIZE / static_cast<double>(graphics.getTransformation());
    const auto dimensions = graphics.getDimensions();

    graphics.setColor(Util::Graphic::Colors::WHITE);
    graphics.drawStringDirect(Util::Math::Vector2<double>(-0.9, -0.875 + (charSize / 2)), "HP:");
    graphics.drawStringDirect(Util::Math::Vector2<double>(0, -0.875 + (charSize / 2)), Util::String::format("DMG: %u", player.getDamage()));

    // Health
    graphics.setColor(Util::Graphic::Color(255, 117, 0));
    graphics.drawRectangleDirect(Util::Math::Vector2<double>(-0.95, -0.95), Util::Math::Vector2<double>(1.9, 0.15));

    graphics.setColor(Util::Graphic::Color(255, 0, 0));
    for (uint32_t i = 0; i < player.getHealth(); i++) {
        graphics.fillRectangleDirect(Util::Math::Vector2<double>(-0.75 + (0.075 * i), -0.9), Util::Math::Vector2<double>(0.05, 0.05));
    }

    // Minimap Background
    graphics.setColor(Util::Graphic::Color(255, 117, 0));
    graphics.drawRectangleDirect(Util::Math::Vector2<double>(dimensions.getX() - 0.425, 0.575), Util::Math::Vector2<double>(0.4, 0.4));

    graphics.setColor(Util::Graphic::Colors::WHITE);
    graphics.drawStringDirect(Util::Math::Vector2<double>(dimensions.getX() - 0.425, 0.575 - charSize), Util::String::format("Level: %u", player.getLevel()));

    // Rooms
    for (uint32_t i = 0; i < 4; i++) {
        for (uint32_t j = 0; j < 4; j++) {
            if (map[i][j] == Room::NONE) {
                continue;
            }

            if (map[i][j] == Room::Type::END) { // Boss rooms are marked with red color on the minimap
                graphics.setColor(Util::Graphic::Color(153,0,0));
            } else {
                graphics.setColor(Util::Graphic::Color(117, 117, 117));
            }

            if (i == currentRow && j == currentColumn) { // Current position will be highlighted with blue color on the minimap
                graphics.setColor(Util::Graphic::Color(68, 195, 212));
            }

            graphics.fillRectangleDirect(Util::Math::Vector2<double>(dimensions.getX() - 0.4 + (0.1 * j), 0.9 - (0.1 * i)), Util::Math::Vector2<double>(0.05, 0.05));
        }
    }
}

void Hud::setCurrentPosition(uint32_t newRow, uint32_t newColumn) {
    currentRow = newRow;
    currentColumn = newColumn;
}

void Hud::onUpdate([[maybe_unused]] double delta) {}

void Hud::onCollisionEvent([[maybe_unused]] Util::Game::D3::CollisionEvent &event) {}
