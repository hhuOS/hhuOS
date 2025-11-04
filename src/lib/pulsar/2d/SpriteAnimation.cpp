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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "Sprite.h"
#include "SpriteAnimation.h"
#include "lib/pulsar/Graphics.h"

namespace Pulsar::D2 {

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

const Util::Math::Vector2<double> &SpriteAnimation::getOriginalSize() const {
    return sprites[currentSprite].getOriginalSize();
}

Util::Math::Vector2<double> SpriteAnimation::getSize() const {
    auto size = sprites[currentSprite].getSize();
    return Util::Math::Vector2<double>(size.getX() * scale.getX(), size.getY() * scale.getY());
}

const Util::Math::Vector2<double> &SpriteAnimation::getScale() const {
    return scale;
}

double SpriteAnimation::getRotation() const {
    return rotationAngle;
}

double SpriteAnimation::getAlpha() const {
    return alpha;
}

void SpriteAnimation::setScale(const Util::Math::Vector2<double> &scale) {
    SpriteAnimation::scale = scale;
}

void SpriteAnimation::setScale(double scale) {
    SpriteAnimation::scale = Util::Math::Vector2<double>(scale, scale);
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

void SpriteAnimation::setXFlipped(bool flipped) {
    xFlipped = flipped;
}

void SpriteAnimation::draw(const Graphics &graphics, const Util::Math::Vector2<double> &position) const {
    graphics.drawImage2D(position, sprites[currentSprite].getImage(), xFlipped, alpha, scale, rotationAngle);
}

void SpriteAnimation::setAlpha(double alpha) {
    SpriteAnimation::alpha = alpha;
}

}