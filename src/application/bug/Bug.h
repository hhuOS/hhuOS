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

#ifndef HHUOS_BUG_H
#define HHUOS_BUG_H

#include <cstdint>

#include "lib/util/game/SpriteAnimation.h"
#include "Explosive.h"

class Fleet;

namespace Util {
namespace Game {
class CollisionEvent;
class Graphics;
class TranslationEvent;
}  // namespace Game
namespace Math {
class Vector2D;
}  // namespace Math
}  // namespace Util

class Bug : public Explosive {

public:
    /**
     * Constructor.
     */
    explicit Bug(const Util::Math::Vector2D &position, Fleet &fleet);

    /**
     * Copy Constructor.
     */
    Bug(const Bug &other) = delete;

    /**
     * Assignment operator.
     */
    Bug &operator=(const Bug &other) = delete;

    /**
     * Destructor.
     */
    ~Bug() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void onTranslationEvent(Util::Game::TranslationEvent &event) override;

    void onCollisionEvent(Util::Game::CollisionEvent &event) override;

    void draw(Util::Game::Graphics &graphics) override;

    void fireMissile();

    static const constexpr uint32_t TAG = 3;
    static const constexpr double SIZE_X = 0.15;
    static const constexpr double SIZE_Y = 0.1;

private:

    Util::Game::SpriteAnimation animation;
    Fleet &fleet;
};

#endif
