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
#include "lib/util/game/Graphics2D.h"
#include "lib/util/game/Sprite.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/game/entity/component/LinearMovementComponent.h"
#include "lib/util/game/entity/component/GravityComponent.h"
#include "lib/util/game/entity/event/CollisionEvent.h"
#include "Ground.h"

Dino::Dino() : Util::Game::Entity(TAG, Util::Math::Vector2D(-0.8, 0), Util::Game::RectangleCollider(Util::Math::Vector2D(-0.8, 0), Util::Game::Collider::DYNAMIC, 0.2, 0.2667)),
        currentAnimation(&eggAnimation),
        idleAnimation(Util::Array<Util::Game::Sprite*>({
        new Util::Game::Sprite("/initrd/dino/idle1.bmp", 0.2, 0.2267),
        new Util::Game::Sprite("/initrd/dino/idle2.bmp", 0.2, 0.2267),
        new Util::Game::Sprite("/initrd/dino/idle3.bmp", 0.2, 0.2267)}), 0.25),
        runAnimation(Util::Array<Util::Game::Sprite*>({
        new Util::Game::Sprite("/initrd/dino/run1.bmp", 0.2, 0.2267),
        new Util::Game::Sprite("/initrd/dino/run2.bmp", 0.2, 0.2267),
        new Util::Game::Sprite("/initrd/dino/run3.bmp", 0.2, 0.2267),
        new Util::Game::Sprite("/initrd/dino/run4.bmp", 0.2, 0.2267),
        new Util::Game::Sprite("/initrd/dino/run5.bmp", 0.2, 0.2267),
        new Util::Game::Sprite("/initrd/dino/run6.bmp", 0.2, 0.2267)}), 0.5),
        dashAnimation(Util::Array<Util::Game::Sprite*>({
        new Util::Game::Sprite("/initrd/dino/dash1.bmp", 0.24, 0.2),
        new Util::Game::Sprite("/initrd/dino/dash2.bmp", 0.24, 0.2),
        new Util::Game::Sprite("/initrd/dino/dash3.bmp", 0.24, 0.2),
        new Util::Game::Sprite("/initrd/dino/dash4.bmp", 0.24, 0.2),
        new Util::Game::Sprite("/initrd/dino/dash5.bmp", 0.24, 0.2),
        new Util::Game::Sprite("/initrd/dino/dash6.bmp", 0.24, 0.2)}), 0.4),
        eggAnimation(Util::Array<Util::Game::Sprite*>({
        new Util::Game::Sprite("/initrd/dino/egg1.bmp", 0.2, 0.2667),
        new Util::Game::Sprite("/initrd/dino/egg2.bmp", 0.2, 0.2667),
        new Util::Game::Sprite("/initrd/dino/egg3.bmp", 0.2, 0.2667),
        new Util::Game::Sprite("/initrd/dino/egg4.bmp", 0.2, 0.2667)}), 2),
        crackAnimation(Util::Array<Util::Game::Sprite*>({
        new Util::Game::Sprite("/initrd/dino/crack1.bmp", 0.2, 0.2667),
        new Util::Game::Sprite("/initrd/dino/crack2.bmp", 0.2, 0.2667),
        new Util::Game::Sprite("/initrd/dino/crack3.bmp", 0.2, 0.2667),
        new Util::Game::Sprite("/initrd/dino/crack4.bmp", 0.2, 0.2667)}), 1),
        hatchAnimation(Util::Array<Util::Game::Sprite*>({
        new Util::Game::Sprite("/initrd/dino/hatch1.bmp", 0.2, 0.2667),
        new Util::Game::Sprite("/initrd/dino/hatch2.bmp", 0.2, 0.2667),
        new Util::Game::Sprite("/initrd/dino/hatch3.bmp", 0.2, 0.2667),
        new Util::Game::Sprite("/initrd/dino/hatch4.bmp", 0.2, 0.2667)}), 0.5),
        deathAnimation(Util::Array<Util::Game::Sprite*>({
        new Util::Game::Sprite("/initrd/dino/death1.bmp", 0.2, 0.2267),
        new Util::Game::Sprite("/initrd/dino/death2.bmp", 0.2, 0.2267),
        new Util::Game::Sprite("/initrd/dino/death3.bmp", 0.2, 0.2267),
        new Util::Game::Sprite("/initrd/dino/death4.bmp", 0.2, 0.2267),
        new Util::Game::Sprite("/initrd/dino/death5.bmp", 0.2, 0.2267)}), 0.5) {}

void Dino::moveLeft() {
    if (hatched && !isDying && !dead) {
        if (direction == RIGHT) {
            setVelocityX(getVelocity().getX() * STOP_FACTOR);
        }

        currentAnimation = dashing ? &dashAnimation : &runAnimation;
        direction = LEFT;
        isMoving = true;
    }
}

void Dino::moveRight() {
    if (hatched && !isDying && !dead) {
        if (direction == LEFT) {
            setVelocityX(getVelocity().getX() * STOP_FACTOR);
        }

        currentAnimation = dashing ? &dashAnimation : &runAnimation;
        direction = RIGHT;
        isMoving = true;
    }
}

void Dino::dash(bool dash) {
    if (hatched && !isDying && !dead) {
        dashing = dash;
    }
}

void Dino::jump() {
    if (onGround && hatched && !isDying && !dead) {
        setVelocityY(JUMP_VELOCITY);
        onGround = false;
    }
}

void Dino::stop() {
    if (hatched && !isDying && !dead) {
        if (onGround) {
            setVelocityX(getVelocity().getX() * STOP_FACTOR);
        }

        currentAnimation = &idleAnimation;
        isMoving = false;
        runAnimation.reset();
        dashAnimation.reset();
    }
}

void Dino::hatch() {
    if (!isHatching && !hatched && !isDying && !dead) {
        currentAnimation = &crackAnimation;
        isHatching = true;
    }
}

void Dino::onUpdate(double delta) {
    if (dead) {
        return;
    }

    if (isHatching) {
        time += delta;
        if (time >= crackAnimation.getAnimationTime() && time < crackAnimation.getAnimationTime() + hatchAnimation.getAnimationTime()) {
            currentAnimation = &hatchAnimation;
        } else if (time > crackAnimation.getAnimationTime() + hatchAnimation.getAnimationTime()) {
            hatched = true;
            isHatching = false;
            currentAnimation = &idleAnimation;
        }
    } else if (isDying) {
        time += delta;
        if (time >= deathAnimation.getAnimationTime() && time < deathAnimation.getAnimationTime() + 0.5) {
            return;
        } else if (time >= deathAnimation.getAnimationTime() + 0.5) {
            isDying = false;
            dead = true;
            return;
        }
    } else if (isMoving) {
        if (direction == LEFT) {
            auto maxVelocity = -targetVelocity;
            auto xVelocity = getVelocity().getX() - MOVEMENT_FACTOR * delta;
            setVelocityX(xVelocity < maxVelocity ? maxVelocity : xVelocity);
        } else if (direction == RIGHT) {
            auto maxVelocity = targetVelocity;
            auto xVelocity = getVelocity().getX() + MOVEMENT_FACTOR * delta;
            setVelocityX(xVelocity > maxVelocity ? maxVelocity : xVelocity);
        }
    } else if (onGround) {
        if (direction == LEFT && getVelocity().getX() < 0) {
            auto xVelocity = getVelocity().getX() + MOVEMENT_FACTOR * delta;
            setVelocityX(xVelocity > 0 ? 0 : xVelocity);
        } else if (direction == RIGHT && getVelocity().getX() > 0) {
            auto xVelocity = getVelocity().getX() - MOVEMENT_FACTOR * delta;
            setVelocityX(xVelocity < 0 ? 0 : xVelocity);
        }
    }

    if (onGround || isDying) {
        currentAnimation->update(delta);
    }
}

void Dino::draw(Util::Game::Graphics2D &graphics) {
    graphics.drawImage(getPosition(), currentAnimation->getCurrentSprite().getImage(), direction == LEFT);
}

void Dino::die() {
    if (hatched && !isDying && !dead) {
        time = 0;
        isDying = true;
        isMoving = false;
        setVelocity(Util::Math::Vector2D(0, 0));
        currentAnimation = &deathAnimation;
    }
}

void Dino::reset() {
    if (!isHatching) {
        time = 0;
        isHatching = false;
        isDying = false;
        isMoving = false;
        dashing = false;
        hatched = false;
        dead = false;
        idleAnimation.reset();
        runAnimation.reset();
        dashAnimation.reset();
        eggAnimation.reset();
        crackAnimation.reset();
        hatchAnimation.reset();
        deathAnimation.reset();
        currentAnimation = &eggAnimation;
        targetVelocity = DEFAULT_MOVE_VELOCITY;
        setPosition(Util::Math::Vector2D(-0.8, 0));
        setVelocity(Util::Math::Vector2D(0, 0));
        currentAnimation = &eggAnimation;
    }
}

void Dino::onTranslationEvent(Util::Game::TranslationEvent &event) {
    if (isDying || dead) {
        event.cancel();
    }
}

void Dino::onCollisionEvent(Util::Game::CollisionEvent &event) {
    if (event.getSide() == Util::Game::RectangleCollider::BOTTOM && event.getCollidedWidth().getTag() == Ground::TAG) {
        onGround = true;
    } else {
        die();
    }
}

bool Dino::hasHatched() const {
    return hatched;
}

bool Dino::isDead() const {
    return dead;
}

double Dino::getTargetVelocity() const {
    return targetVelocity;
}

void Dino::setTargetVelocity(double targetVelocity) {
    Dino::targetVelocity = targetVelocity;
}
