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
 */

#ifndef HHUOS_PLAYERMISSILE_H
#define HHUOS_PLAYERMISSILE_H

#include <stdint.h>

#include "lib/util/game/2d/Entity.h"
#include "lib/util/game/2d/Sprite.h"

class Ship;
namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math
}  // namespace Util

class PlayerMissile : public Util::Game::D2::Entity {

public:
    /**
     * Constructor.
     */
    PlayerMissile(const Util::Math::Vector2<double> &position, Ship &ship);

    /**
     * Copy Constructor.
     */
    PlayerMissile(const PlayerMissile &other) = delete;

    /**
     * Assignment operator.
     */
    PlayerMissile &operator=(const PlayerMissile &other) = delete;

    /**
     * Destructor.
     */
    ~PlayerMissile() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void onTranslationEvent(Util::Game::D2::TranslationEvent &event) override;

    void onCollisionEvent(Util::Game::D2::CollisionEvent &event) override;

    void draw(Util::Game::Graphics &graphics) override;

    static const constexpr uint32_t TAG = 1;
    static const constexpr double SIZE_X = 0.03;
    static const constexpr double SIZE_Y = 0.065;

private:

    Util::Game::D2::Sprite sprite;
    Ship &ship;
};

#endif
