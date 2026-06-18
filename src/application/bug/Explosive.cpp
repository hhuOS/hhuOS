/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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
 */

#include "Explosive.h"

#include <util/collection/Array.h>
#include <util/base/String.h>
#include <pulsar/audio/AudioHandle.h>
#include <pulsar/2d/Sprite.h>
#include <pulsar/2d/collider/RectangleCollider.h>

Explosive::Explosive(const size_t tag, const Util::Math::Vector2<float> &position,
    const Pulsar::D2::RectangleCollider &collider, const Util::String &waveFilePath, const float animationTime) :
    Entity(tag, position, collider), waveFilePath(waveFilePath), animationTime(animationTime) {}

void Explosive::initialize() {
    soundEffect = Pulsar::AudioTrack(waveFilePath);

    const auto &collider = getCollider();
    const auto width = collider.getWidth();
    const auto height = collider.getHeight();
    const auto size = height > width ? height : width;

    animation = Pulsar::D2::SpriteAnimation(Util::Array<Pulsar::D2::Sprite>({
        Pulsar::D2::Sprite("/user/bug/explosion1.bmp", size, size),
        Pulsar::D2::Sprite("/user/bug/explosion2.bmp", size, size),
        Pulsar::D2::Sprite("/user/bug/explosion3.bmp", size, size),
        Pulsar::D2::Sprite("/user/bug/explosion4.bmp", size, size),
        Pulsar::D2::Sprite("/user/bug/explosion5.bmp", size, size),
        Pulsar::D2::Sprite("/user/bug/explosion6.bmp", size, size),
        Pulsar::D2::Sprite("/user/bug/explosion7.bmp", size, size),
        Pulsar::D2::Sprite("/user/bug/explosion8.bmp", size, size)
    }), animationTime);
}

void Explosive::onUpdate(const float delta) {
    if (shouldExplode) {
        // Explosion has just started -> Play the sound effect and set `exploding` to true.
        exploding = true;
        shouldExplode = false;
        soundEffectHandle = soundEffect.play(false);
    } else if (exploding) {
        // Explosion is in progress -> Update the animation
        explosionTimer += delta;
        if (explosionTimer <= animation.getAnimationTime()) {
            animation.update(delta);
        }
    }
}

void Explosive::draw(Pulsar::Graphics &graphics) const {
    animation.draw(graphics, getPosition());
}
