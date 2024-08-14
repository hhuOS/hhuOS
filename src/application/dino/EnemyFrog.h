/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
 *
 * The dino game is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#ifndef HHUOS_ENEMYFROG_H
#define HHUOS_ENEMYFROG_H

#include "lib/util/game/2d/Entity.h"
#include "lib/util/game/2d/SpriteAnimation.h"

class EnemyFrog : public Util::Game::D2::Entity {

public:
    /**
     * Constructor.
     */
    EnemyFrog(const Util::Math::Vector2D &position);

    /**
     * Copy Constructor.
     */
    EnemyFrog(const EnemyFrog &other) = delete;

    /**
     * Assignment operator.
     */
    EnemyFrog &operator=(const EnemyFrog &other) = delete;

    /**
     * Destructor.
     */
    ~EnemyFrog() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void draw(Util::Game::Graphics &graphics) override;

    void onTranslationEvent(Util::Game::D2::TranslationEvent &event) override;

    void onCollisionEvent(Util::Game::D2::CollisionEvent &event) override;

    static const constexpr uint32_t TAG = 7;

private:

    enum Direction {
        LEFT, RIGHT
    };

    Util::Game::D2::SpriteAnimation animation;
    Direction direction = LEFT;

    static const constexpr double SIZE = 0.069;
    static const constexpr double VELOCITY = 0.25;
};

#endif
