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

#include "Sprite.h"
#include "SpriteAnimation.h"

namespace Util::Game {

SpriteAnimation::SpriteAnimation() : sprites(0) {}

SpriteAnimation::SpriteAnimation(const Util::Array<Sprite> &sprites, double time) : animationTime(time), timePerSprite(time / sprites.length()), sprites(sprites) {}

void SpriteAnimation::reset() {
    currentSprite = 0;
    timeSinceLastChange = 0;
}

void SpriteAnimation::update(double delta) {
    timeSinceLastChange += delta;
    auto advancedFrames = static_cast<uint32_t>(timeSinceLastChange / timePerSprite);
    if (advancedFrames > 0) {
        currentSprite = (currentSprite + advancedFrames) % sprites.length();
        timeSinceLastChange -= advancedFrames * timePerSprite;
    }
}

const Sprite& SpriteAnimation::getCurrentSprite() const {
    return sprites[currentSprite];
}

double SpriteAnimation::getAnimationTime() const {
    return animationTime;
}

double SpriteAnimation::getWidth() const {
    return sprites[currentSprite].getWidth();
}

double SpriteAnimation::getHeight() const {
    return sprites[currentSprite].getHeight();
}

}