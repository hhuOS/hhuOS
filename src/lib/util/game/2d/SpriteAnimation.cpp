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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "Sprite.h"
#include "SpriteAnimation.h"
#include "lib/util/game/Graphics.h"

namespace Util::Game::D2 {

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

double SpriteAnimation::getAnimationTime() const {
    return animationTime;
}

const Math::Vector2D &SpriteAnimation::getOriginalSize() const {
    return sprites[currentSprite].getOriginalSize();
}

Math::Vector2D SpriteAnimation::getSize() const {
    auto size = sprites[currentSprite].getSize();
    return Math::Vector2D(size.getX() * scale.getX(), size.getY() * scale.getY());
}

const Math::Vector2D &SpriteAnimation::getScale() const {
    return scale;
}

double SpriteAnimation::getRotation() const {
    return rotationAngle;
}

double SpriteAnimation::getAlpha() const {
    return alpha;
}

void SpriteAnimation::setScale(const Math::Vector2D &scale) {
    SpriteAnimation::scale = scale;
}

void SpriteAnimation::setScale(double scale) {
    SpriteAnimation::scale = Math::Vector2D(scale, scale);
}

void SpriteAnimation::setRotation(double angle) {
    rotationAngle = angle;
}

void SpriteAnimation::rotate(double angle) {
    rotationAngle += angle;
}

void SpriteAnimation::flipX() {
    xFlipped = !xFlipped;
}

void SpriteAnimation::draw(const Graphics &graphics, const Math::Vector2D &position) const {
    graphics.drawImage2D(position, sprites[currentSprite].getImage(), xFlipped, alpha, scale, rotationAngle);
}

void SpriteAnimation::setAlpha(double alpha) {
    SpriteAnimation::alpha = alpha;
}

}