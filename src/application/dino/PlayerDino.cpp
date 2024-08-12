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
 */

#include "PlayerDino.h"

#include "lib/util/collection/Array.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/game/2d/Sprite.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/game/2d/event/CollisionEvent.h"
#include "Ground.h"
#include "lib/util/game/Collider.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/game/2d/event/TranslationEvent.h"
#include "lib/util/base/String.h"
#include "lib/util/graphic/Colors.h"
#include "Saw.h"
#include "lib/util/graphic/font/Terminal8x16.h"

PlayerDino::PlayerDino(const Util::Math::Vector2D &position) : Util::Game::D2::Entity(TAG, position, Util::Game::D2::RectangleCollider(position, Util::Math::Vector2D(0.2, 0.2), Util::Game::Collider::DYNAMIC)) {}

void PlayerDino::initialize() {
    runAnimation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/dino/run1.bmp", 0.2, 0.2267),
        Util::Game::D2::Sprite("/user/dino/run2.bmp", 0.2, 0.2267),
        Util::Game::D2::Sprite("/user/dino/run3.bmp", 0.2, 0.2267),
        Util::Game::D2::Sprite("/user/dino/run4.bmp", 0.2, 0.2267),
        Util::Game::D2::Sprite("/user/dino/run5.bmp", 0.2, 0.2267),
        Util::Game::D2::Sprite("/user/dino/run6.bmp", 0.2, 0.2267)}), 0.5);
    dashAnimation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/dino/dash1.bmp", 0.24, 0.2),
        Util::Game::D2::Sprite("/user/dino/dash2.bmp", 0.24, 0.2),
        Util::Game::D2::Sprite("/user/dino/dash3.bmp", 0.24, 0.2),
        Util::Game::D2::Sprite("/user/dino/dash4.bmp", 0.24, 0.2),
        Util::Game::D2::Sprite("/user/dino/dash5.bmp", 0.24, 0.2),
        Util::Game::D2::Sprite("/user/dino/dash6.bmp", 0.24, 0.2)}), 0.4);
    eggAnimation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/dino/egg1.bmp", 0.2, 0.2),
        Util::Game::D2::Sprite("/user/dino/egg2.bmp", 0.2, 0.2),
        Util::Game::D2::Sprite("/user/dino/egg3.bmp", 0.2, 0.2),
        Util::Game::D2::Sprite("/user/dino/egg4.bmp", 0.2, 0.2),
        Util::Game::D2::Sprite("/user/dino/egg5.bmp", 0.2, 0.2),
        Util::Game::D2::Sprite("/user/dino/egg6.bmp", 0.2, 0.2)}), 0.5);
    crackAnimation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/dino/crack1.bmp", 0.2, 0.2667),
        Util::Game::D2::Sprite("/user/dino/crack2.bmp", 0.2, 0.2667),
        Util::Game::D2::Sprite("/user/dino/crack3.bmp", 0.2, 0.2667),
        Util::Game::D2::Sprite("/user/dino/crack4.bmp", 0.2, 0.2667)}), 1.0);
    hatchAnimation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/dino/hatch1.bmp", 0.2, 0.2667),
        Util::Game::D2::Sprite("/user/dino/hatch2.bmp", 0.2, 0.2667),
        Util::Game::D2::Sprite("/user/dino/hatch3.bmp", 0.2, 0.2667),
        Util::Game::D2::Sprite("/user/dino/hatch4.bmp", 0.2, 0.2667)}), 0.5);
    deathAnimation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/dino/death1.bmp", 0.2, 0.2267),
        Util::Game::D2::Sprite("/user/dino/death2.bmp", 0.2, 0.2267),
        Util::Game::D2::Sprite("/user/dino/death3.bmp", 0.2, 0.2267),
        Util::Game::D2::Sprite("/user/dino/death4.bmp", 0.2, 0.2267),
        Util::Game::D2::Sprite("/user/dino/death5.bmp", 0.2, 0.2267)}), 0.5);
}

void PlayerDino::dash() {
    currentAnimation = &dashAnimation;
}

void PlayerDino::jump() {
    if (onGround && hatched && !dying && !dead) {
        setVelocityY(JUMP_VELOCITY);
        onGround = false;
    }
}

void PlayerDino::hatch() {
    if (!hatching && !hatched && !dying && !dead) {
        currentAnimation = &crackAnimation;
        hatching = true;
    }
}

void PlayerDino::onUpdate(double delta) {
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

    getCollider().setSize(currentAnimation->getOriginalSize());
}

void PlayerDino::draw(Util::Game::Graphics &graphics) {
    currentAnimation->draw(graphics, getPosition());

    graphics.setColor(Util::Graphic::Colors::GREEN);
    graphics.drawString(Util::Graphic::Fonts::TERMINAL_8x16, Util::Math::Vector2D(10, 10), Util::String::format("Points: %u", points));
}

void PlayerDino::die() {
    if (hatched && !dying && !dead) {
        time = 0;
        dying = true;
        setVelocity(Util::Math::Vector2D(0, 0));
        currentAnimation = &deathAnimation;
    }
}

void PlayerDino::reset() {
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

void PlayerDino::onTranslationEvent(Util::Game::D2::TranslationEvent &event) {
    if (dying || dead) {
        event.cancel();
    }
}

void PlayerDino::onCollisionEvent(Util::Game::D2::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == Ground::TAG) {
        onGround = true;
    } else if(event.getCollidedWidth().getTag() == Saw::TAG) {
        die();
    }
}

bool PlayerDino::hasHatched() const {
    return hatched;
}

bool PlayerDino::isDying() const {
    return dying;
}

bool PlayerDino::isDead() const {
    return dead;
}

void PlayerDino::setPoints(uint32_t points) {
    PlayerDino::points = points;
}

uint32_t PlayerDino::getPoints() const {
    return points;
}
