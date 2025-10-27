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
 */

#include "Explosive.h"

#include "game/Game.h"
#include "game/audio/AudioHandle.h"
#include "lib/util/collection/Array.h"
#include "lib/util/game/2d/Sprite.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/base/String.h"

namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math
}  // namespace Util

Explosive::Explosive(uint32_t tag, const Util::Math::Vector2<double> &position, const Util::Game::D2::RectangleCollider &collider, const Util::String &waveFilePath, double animationTime) : Entity(tag, position, collider), waveFilePath(waveFilePath), animationTime(animationTime) {}

void Explosive::initialize() {
    soundEffect = Util::Game::AudioTrack(waveFilePath);

    auto size = getCollider().getHeight() > getCollider().getWidth() ? getCollider().getHeight() : getCollider().getWidth();
    animation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/bug/explosion1.bmp", size, size),
        Util::Game::D2::Sprite("/user/bug/explosion2.bmp", size, size),
        Util::Game::D2::Sprite("/user/bug/explosion3.bmp", size, size),
        Util::Game::D2::Sprite("/user/bug/explosion4.bmp", size, size),
        Util::Game::D2::Sprite("/user/bug/explosion5.bmp", size, size),
        Util::Game::D2::Sprite("/user/bug/explosion6.bmp", size, size),
        Util::Game::D2::Sprite("/user/bug/explosion7.bmp", size, size),
        Util::Game::D2::Sprite("/user/bug/explosion8.bmp", size, size)}), animationTime);
}

void Explosive::onUpdate(double delta) {
    if (shouldExplode) {
        exploding = true;
        shouldExplode = false;
        soundEffectHandle = soundEffect.play(false);
    } else if (exploding) {
        explosionTimer += delta;
        if (explosionTimer <= animation.getAnimationTime()) {
            animation.update(delta);
        }
    }
}

void Explosive::draw(Util::Game::Graphics &graphics) const {
    animation.draw(graphics, getPosition());
}

void Explosive::explode() {
    if (!exploding) {
        shouldExplode = true;
    }
}

bool Explosive::isExploding() const {
    return exploding;
}

bool Explosive::hasExploded() const {
    return explosionTimer >= animation.getAnimationTime() && !soundEffectHandle.isPlaying();
}
