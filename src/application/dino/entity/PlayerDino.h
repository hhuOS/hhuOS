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

#ifndef HHUOS_PLAYERDINO_H
#define HHUOS_PLAYERDINO_H

#include <stdint.h>

#include "lib/util/game/2d/SpriteAnimation.h"
#include "lib/util/game/2d/Entity.h"
#include "application/dino/particle/GrassEmitter.h"
#include "lib/util/time/Timestamp.h"

namespace Util {
namespace Graphic {
class Font;
}  // namespace Graphic

namespace Math {
class Vector2D;
}  // namespace Math

namespace Game {
class Graphics;

namespace D2 {
class CollisionEvent;
class TranslationEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

class PlayerDino : public Util::Game::D2::Entity {

public:

    enum Direction : int32_t {
        RIGHT = 1,
        LEFT = -1
    };

    /**
     * Constructor.
     */
    explicit PlayerDino(const Util::Math::Vector2D &position);

    /**
     * Copy Constructor.
     */
    PlayerDino(const PlayerDino &other) = delete;

    /**
     * Assignment operator.
     */
    PlayerDino &operator=(const PlayerDino &other) = delete;

    /**
     * Destructor.
     */
    ~PlayerDino() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void idle();

    void run();

    void jump();

    void hatch();

    void die();

    void reset();

    void setDirection(Direction direction);

    [[nodiscard]] Direction getDirection() const;

    [[nodiscard]] bool hasHatched() const;

    [[nodiscard]] bool isDying() const;

    [[nodiscard]] bool isDead() const;

    [[nodiscard]] uint32_t getPoints() const;

    void setPoints(uint32_t points);

    void addPoints(uint32_t points);

    void onTranslationEvent(Util::Game::D2::TranslationEvent &event) override;

    void onCollisionEvent(Util::Game::D2::CollisionEvent &event) override;

    void draw(Util::Game::Graphics &graphics) override;

    static const constexpr uint32_t TAG = 0;
    static const constexpr double SIZE = 0.1;

private:

    bool hatching = false;
    bool dying = false;
    bool hatched = false;
    bool dead = false;
    bool onGround = false;
    bool running = false;

    Direction direction = RIGHT;

    uint32_t points = 0;

    double time = 0;
    Util::Game::D2::SpriteAnimation *currentAnimation = &eggAnimation;
    Util::Game::D2::SpriteAnimation idleAnimation;
    Util::Game::D2::SpriteAnimation runAnimation;
    Util::Game::D2::SpriteAnimation eggAnimation;
    Util::Game::D2::SpriteAnimation crackAnimation;
    Util::Game::D2::SpriteAnimation hatchAnimation;
    Util::Game::D2::SpriteAnimation deathAnimation;

    const Util::Graphic::Font &pointsFont;

    GrassEmitter *grassEmitter = new GrassEmitter(*this);
    Util::Time::Timestamp lastEmissionTime;

    static const constexpr double MAX_VELOCITY = 0.3;
    static const constexpr double JUMP_VELOCITY = 0.75;
    static const constexpr uint32_t EMISSION_INTERVAL_MS = 500;
};

#endif
