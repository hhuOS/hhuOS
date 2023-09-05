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

#ifndef HHUOS_DEMOSPRITE_H
#define HHUOS_DEMOSPRITE_H

#include <cstdint>

#include "lib/util/game/2d/Entity.h"
#include "lib/util/game/SpriteAnimation.h"
#include "lib/util/math/Vector2D.h"

namespace Util {
namespace Game {
class Graphics;
namespace D2 {
class CollisionEvent;
class TranslationEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

class DemoSprite : public Util::Game::D2::Entity {

public:
    /**
     * Default Constructor.
     */
    DemoSprite(const Util::Math::Vector2D &position, double size, double rotationSpeed, double scaleSpeed, bool flipX);

    /**
     * Copy Constructor.
     */
    DemoSprite(const DemoSprite &other) = delete;

    /**
     * Assignment operator.
     */
    DemoSprite &operator=(const DemoSprite &other) = delete;

    /**
     * Destructor.
     */
    ~DemoSprite() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void draw(Util::Game::Graphics &graphics) override;

    void onTranslationEvent(Util::Game::D2::TranslationEvent &event) override;

    void onCollisionEvent(Util::Game::D2::CollisionEvent &event) override;

    static const constexpr uint32_t TAG = 0;

private:

    Util::Game::SpriteAnimation animation;

    Util::Math::Vector2D initialPosition;
    double size;
    double rotationSpeed;
    double scaleSpeed;
    bool flipX;
    bool scaleUp = true;
};


#endif
