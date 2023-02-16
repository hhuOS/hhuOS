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

#ifndef HHUOS_DINO_H
#define HHUOS_DINO_H

#include "lib/util/game/SpriteAnimation.h"
#include "lib/util/game/entity/Entity.h"

namespace Util {
namespace Game {
class Graphics2D;
}  // namespace Game
}  // namespace Util

class Dino : public Util::Game::Entity {

public:
    /**
     * Default Constructor.
     */
    Dino();

    /**
     * Copy Constructor.
     */
    Dino(const Dino &other) = delete;

    /**
     * Assignment operator.
     */
    Dino &operator=(const Dino &other) = delete;

    /**
     * Destructor.
     */
    ~Dino() override = default;

    void moveLeft();

    void moveRight();

    void dash(bool dash);

    void hatch();

    void stop();

    void die();

    void reset();

    void onUpdate(double delta) override;

    void draw(Util::Game::Graphics2D &graphics) const override;

private:

    bool isHatching = false;
    bool isDying = false;
    bool hatched = false;
    bool invert = false;
    bool dashing = false;

    double time = 0;

    Util::Game::SpriteAnimation *currentAnimation;
    Util::Game::SpriteAnimation idleAnimation;
    Util::Game::SpriteAnimation runAnimation;
    Util::Game::SpriteAnimation dashAnimation;
    Util::Game::SpriteAnimation eggAnimation;
    Util::Game::SpriteAnimation crackAnimation;
    Util::Game::SpriteAnimation hatchAnimation;
    Util::Game::SpriteAnimation deathAnimation;
};

#endif
