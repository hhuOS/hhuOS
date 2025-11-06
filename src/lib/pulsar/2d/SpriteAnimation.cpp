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
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#include "SpriteAnimation.h"

#include "pulsar/Graphics.h"
#include "pulsar/2d/Sprite.h"

namespace Pulsar::D2 {

SpriteAnimation::SpriteAnimation() : sprites(0) {}

SpriteAnimation::SpriteAnimation(const Util::Array<Sprite> &sprites, const double time) :
    animationTime(time), timePerSprite(time / sprites.length()), sprites(sprites) {}

void SpriteAnimation::reset() {
    currentSprite = 0;
    timeSinceLastChange = 0;
}

void SpriteAnimation::update(const double delta) {
    timeSinceLastChange += delta;

    const auto advancedFrames = static_cast<size_t>(timeSinceLastChange / timePerSprite);
    if (advancedFrames > 0) {
        currentSprite = (currentSprite + advancedFrames) % sprites.length();
        timeSinceLastChange -= advancedFrames * timePerSprite;
    }
}

double SpriteAnimation::getAnimationTime() const {
    return animationTime;
}

const Util::Math::Vector2<double>& SpriteAnimation::getOriginalSize() const {
    return sprites[currentSprite].getOriginalSize();
}

Util::Math::Vector2<double> SpriteAnimation::getSize() const {
    const auto size = sprites[currentSprite].getOriginalSize();
    return Util::Math::Vector2<double>(size.getX() * scale.getX(), size.getY() * scale.getY());
}

void SpriteAnimation::setScale(double scale) {
    setScale(Util::Math::Vector2<double>(scale, scale));
}

void SpriteAnimation::setScale(const Util::Math::Vector2<double> &scale) {
    SpriteAnimation::scale = scale;
}

const Util::Math::Vector2<double> &SpriteAnimation::getScale() const {
    return scale;
}

void SpriteAnimation::setRotation(double angle) {
    rotationAngle = angle;
}

double SpriteAnimation::getRotation() const {
    return rotationAngle;
}

void SpriteAnimation::setAlpha(const double alpha) {
    SpriteAnimation::alpha = alpha;
}

double SpriteAnimation::getAlpha() const {
    return alpha;
}

void SpriteAnimation::rotate(const double angle) {
    rotationAngle += angle;
}

void SpriteAnimation::flipX() {
    xFlipped = !xFlipped;
}

void SpriteAnimation::setXFlipped(const bool flipped) {
    xFlipped = flipped;
}

void SpriteAnimation::draw(const Graphics &graphics, const Util::Math::Vector2<double> &position) const {
    graphics.drawImage2D(position, sprites[currentSprite].getImage(), xFlipped, alpha, scale, rotationAngle);
}

}