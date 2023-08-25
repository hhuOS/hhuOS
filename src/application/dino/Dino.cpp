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

#include "Dino.h"

#include "lib/util/collection/Array.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/game/Sprite.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/game/2d/event/CollisionEvent.h"
#include "Ground.h"
#include "lib/util/game/Collider.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/game/2d/event/TranslationEvent.h"

Dino::Dino(const Util::Math::Vector2D &position) : Util::Game::D2::Entity(TAG, position, Util::Game::D2::RectangleCollider(position, Util::Game::Collider::DYNAMIC, 0.2, 0.2)) {}

void Dino::initialize() {
    runAnimation = Util::Game::SpriteAnimation(Util::Array<Util::Game::Sprite>({
        Util::Game::Sprite("/initrd/dino/run1.bmp", 0.2, 0.2267),
        Util::Game::Sprite("/initrd/dino/run2.bmp", 0.2, 0.2267),
        Util::Game::Sprite("/initrd/dino/run3.bmp", 0.2, 0.2267),
        Util::Game::Sprite("/initrd/dino/run4.bmp", 0.2, 0.2267),
        Util::Game::Sprite("/initrd/dino/run5.bmp", 0.2, 0.2267),
        Util::Game::Sprite("/initrd/dino/run6.bmp", 0.2, 0.2267)}), 0.5);
    dashAnimation = Util::Game::SpriteAnimation(Util::Array<Util::Game::Sprite>({
        Util::Game::Sprite("/initrd/dino/dash1.bmp", 0.24, 0.2),
        Util::Game::Sprite("/initrd/dino/dash2.bmp", 0.24, 0.2),
        Util::Game::Sprite("/initrd/dino/dash3.bmp", 0.24, 0.2),
        Util::Game::Sprite("/initrd/dino/dash4.bmp", 0.24, 0.2),
        Util::Game::Sprite("/initrd/dino/dash5.bmp", 0.24, 0.2),
        Util::Game::Sprite("/initrd/dino/dash6.bmp", 0.24, 0.2)}), 0.4);
    eggAnimation = Util::Game::SpriteAnimation(Util::Array<Util::Game::Sprite>({
        Util::Game::Sprite("/initrd/dino/egg1.bmp", 0.2, 0.2),
        Util::Game::Sprite("/initrd/dino/egg2.bmp", 0.2, 0.2),
        Util::Game::Sprite("/initrd/dino/egg3.bmp", 0.2, 0.2),
        Util::Game::Sprite("/initrd/dino/egg4.bmp", 0.2, 0.2),
        Util::Game::Sprite("/initrd/dino/egg5.bmp", 0.2, 0.2),
        Util::Game::Sprite("/initrd/dino/egg6.bmp", 0.2, 0.2)}), 0.5);
    crackAnimation = Util::Game::SpriteAnimation(Util::Array<Util::Game::Sprite>({
        Util::Game::Sprite("/initrd/dino/crack1.bmp", 0.2, 0.2667),
        Util::Game::Sprite("/initrd/dino/crack2.bmp", 0.2, 0.2667),
        Util::Game::Sprite("/initrd/dino/crack3.bmp", 0.2, 0.2667),
        Util::Game::Sprite("/initrd/dino/crack4.bmp", 0.2, 0.2667)}), 1.0);
    hatchAnimation = Util::Game::SpriteAnimation(Util::Array<Util::Game::Sprite>({
        Util::Game::Sprite("/initrd/dino/hatch1.bmp", 0.2, 0.2667),
        Util::Game::Sprite("/initrd/dino/hatch2.bmp", 0.2, 0.2667),
        Util::Game::Sprite("/initrd/dino/hatch3.bmp", 0.2, 0.2667),
        Util::Game::Sprite("/initrd/dino/hatch4.bmp", 0.2, 0.2667)}), 0.5);
    deathAnimation = Util::Game::SpriteAnimation(Util::Array<Util::Game::Sprite>({
        Util::Game::Sprite("/initrd/dino/death1.bmp", 0.2, 0.2267),
        Util::Game::Sprite("/initrd/dino/death2.bmp", 0.2, 0.2267),
        Util::Game::Sprite("/initrd/dino/death3.bmp", 0.2, 0.2267),
        Util::Game::Sprite("/initrd/dino/death4.bmp", 0.2, 0.2267),
        Util::Game::Sprite("/initrd/dino/death5.bmp", 0.2, 0.2267)}), 0.5);
}

void Dino::dash() {
    currentAnimation = &dashAnimation;
}

void Dino::jump() {
    if (onGround && hatched && !dying && !dead) {
        setVelocityY(JUMP_VELOCITY);
        onGround = false;
    }
}

void Dino::hatch() {
    if (!hatching && !hatched && !dying && !dead) {
        currentAnimation = &crackAnimation;
        hatching = true;
    }
}

void Dino::onUpdate(double delta) {
    if (dead) {
        return;
    }

    if (hatching) {
        time += delta;
        if (time >= crackAnimation.getAnimationTime() && time < crackAnimation.getAnimationTime() + hatchAnimation.getAnimationTime()) {
            currentAnimation = &hatchAnimation;
        } else if (time > crackAnimation.getAnimationTime() + hatchAnimation.getAnimationTime()) {
            hatched = true;
            hatching = false;
            currentAnimation = &runAnimation;
        }
    } else if (dying) {
        time += delta;
        if (time >= deathAnimation.getAnimationTime() && time < deathAnimation.getAnimationTime() + 0.5) {
            return;
        } else if (time >= deathAnimation.getAnimationTime() + 0.5) {
            dying = false;
            dead = true;
            return;
        }
    }

    if (onGround || dying) {
        currentAnimation->update(delta);
    }

    getCollider().setWidth(currentAnimation->getWidth());
    getCollider().setHeight(currentAnimation->getHeight());
}

void Dino::draw(Util::Game::Graphics &graphics) {
    graphics.drawImage2D(getPosition(), currentAnimation->getCurrentSprite().getImage());

    graphics.setColor(Util::Graphic::Colors::GREEN);
    graphics.drawString(Util::Math::Vector2D(10, 10), Util::String::format("Points: %u", points));
}

void Dino::die() {
    if (hatched && !dying && !dead) {
        time = 0;
        dying = true;
        setVelocity(Util::Math::Vector2D(0, 0));
        currentAnimation = &deathAnimation;
    }
}

void Dino::reset() {
    if (!hatching) {
        time = 0;
        hatching = false;
        dying = false;
        hatched = false;
        dead = false;
        runAnimation.reset();
        dashAnimation.reset();
        eggAnimation.reset();
        crackAnimation.reset();
        hatchAnimation.reset();
        deathAnimation.reset();
        currentAnimation = &eggAnimation;
        setVelocity(Util::Math::Vector2D(0, 0));
    }
}

void Dino::onTranslationEvent(Util::Game::D2::TranslationEvent &event) {
    if (dying || dead) {
        event.cancel();
    }
}

void Dino::onCollisionEvent(Util::Game::D2::CollisionEvent &event) {
    if (event.getSide() == Util::Game::D2::RectangleCollider::BOTTOM && event.getCollidedWidth().getTag() == Ground::TAG) {
        onGround = true;
    } else {
        die();
    }
}

bool Dino::hasHatched() const {
    return hatched;
}

bool Dino::isDying() const {
    return dying;
}

bool Dino::isDead() const {
    return dead;
}

void Dino::setPoints(uint32_t points) {
    Dino::points = points;
}
