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
 *
 * The network stack is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-maseh100
 */

#ifndef HHUOS_SPRITEANIMATION_H
#define HHUOS_SPRITEANIMATION_H

#include <cstdint>

#include "lib/util/collection/Array.h"
#include "lib/util/game/Sprite.h"

namespace Util::Game {

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

    [[nodiscard]] const Sprite& getCurrentSprite() const;

    [[nodiscard]] double getAnimationTime() const;

    [[nodiscard]] double getWidth() const;

    [[nodiscard]] double getHeight() const;

private:

    double animationTime = 0;
    double timePerSprite = 0;
    double timeSinceLastChange = 0;
    uint32_t currentSprite = 0;
    Array<Sprite> sprites;
};

}

#endif
