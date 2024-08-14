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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_SPRITEANIMATION_H
#define HHUOS_SPRITEANIMATION_H

#include <stdint.h>

#include "lib/util/collection/Array.h"
#include "lib/util/game/2d/Sprite.h"
#include "lib/util/math/Vector2D.h"

namespace Util {
namespace Game {
class Graphics;
}  // namespace Game
}  // namespace Util

namespace Util::Game::D2 {

class SpriteAnimation {

public:
    /**
     * Default Constructor.
     */
    SpriteAnimation();

    /**
     * Constructor.
     */
    SpriteAnimation(const Array<Sprite> &sprites, double time);

    /**
     * Copy Constructor.
     */
    SpriteAnimation(const SpriteAnimation &other) = default;

    /**
     * Assignment operator.
     */
    SpriteAnimation &operator=(const SpriteAnimation &other) = default;

    /**
     * Destructor.
     */
    ~SpriteAnimation() = default;

    void reset();

    void update(double delta);

    [[nodiscard]] double getAnimationTime() const;

    [[nodiscard]] const Math::Vector2D& getScale() const;

    [[nodiscard]] const Math::Vector2D& getOriginalSize() const;

    [[nodiscard]] Math::Vector2D getSize() const;

    [[nodiscard]] double getRotation() const;

    [[nodiscard]] double getAlpha() const;

    void setScale(const Math::Vector2D &scale);

    void setScale(double scale);

    void setRotation(double angle);

    void setAlpha(double alpha);

    void rotate(double angle);

    void flipX();

    void setXFlipped(bool flipped);

    void draw(const Graphics &graphics, const Math::Vector2D &position) const;

private:

    Math::Vector2D scale = Math::Vector2D(1, 1);
    double rotationAngle = 0;
    double alpha = 1;
    bool xFlipped = false;

    double animationTime = 0;
    double timePerSprite = 0;
    double timeSinceLastChange = 0;
    uint32_t currentSprite = 0;
    Array<Sprite> sprites;
};

}

#endif
