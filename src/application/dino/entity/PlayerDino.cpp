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
 *
 * The dino game is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#include "PlayerDino.h"

#include "lib/util/collection/Array.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/game/2d/Sprite.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/game/2d/event/CollisionEvent.h"
#include "lib/util/game/Collider.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/game/2d/event/TranslationEvent.h"
#include "lib/util/base/String.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Scene.h"
#include "Block.h"
#include "lib/util/game/2d/component/LinearMovementComponent.h"
#include "lib/util/game/2d/component/GravityComponent.h"
#include "application/dino/GameOverScreen.h"
#include "lib/util/graphic/Font.h"

PlayerDino::PlayerDino(const Util::Math::Vector2D &position) :
        Util::Game::D2::Entity(TAG, position, Util::Game::D2::RectangleCollider(position, Util::Math::Vector2D(SIZE, SIZE * 1.133), Util::Game::Collider::DYNAMIC)),
        pointsFont(Util::Graphic::Font::getFontForResolution(static_cast<uint32_t>(Util::Game::GameManager::getAbsoluteResolution().getY()))) {
    Util::Game::GameManager::getCurrentScene().addObject(grassEmitter);
}

void PlayerDino::initialize() {
    idleAnimation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
       Util::Game::D2::Sprite("/user/dino/player/idle1.bmp", SIZE, SIZE * 1.333),
       Util::Game::D2::Sprite("/user/dino/player/idle2.bmp", SIZE, SIZE * 1.333),
       Util::Game::D2::Sprite("/user/dino/player/idle3.bmp", SIZE, SIZE * 1.333)}), 0.5);
    runAnimation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/dino/player/run1.bmp", SIZE, SIZE * 1.133),
        Util::Game::D2::Sprite("/user/dino/player/run2.bmp", SIZE, SIZE * 1.133),
        Util::Game::D2::Sprite("/user/dino/player/run3.bmp", SIZE, SIZE * 1.133),
        Util::Game::D2::Sprite("/user/dino/player/run4.bmp", SIZE, SIZE * 1.133),
        Util::Game::D2::Sprite("/user/dino/player/run5.bmp", SIZE, SIZE * 1.133),
        Util::Game::D2::Sprite("/user/dino/player/run6.bmp", SIZE, SIZE * 1.133)}), 0.5);
    eggAnimation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/dino/player/egg1.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/player/egg2.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/player/egg3.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/player/egg4.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/player/egg5.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/player/egg6.bmp", SIZE, SIZE)}), 0.5);
    crackAnimation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/dino/player/crack1.bmp", SIZE, SIZE * 1.333),
        Util::Game::D2::Sprite("/user/dino/player/crack2.bmp", SIZE, SIZE * 1.333),
        Util::Game::D2::Sprite("/user/dino/player/crack3.bmp", SIZE, SIZE * 1.333),
        Util::Game::D2::Sprite("/user/dino/player/crack4.bmp", SIZE, SIZE * 1.333)}), 1.0);
    hatchAnimation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/dino/player/hatch1.bmp", SIZE, SIZE * 1.333),
        Util::Game::D2::Sprite("/user/dino/player/hatch2.bmp", SIZE, SIZE * 1.333),
        Util::Game::D2::Sprite("/user/dino/player/hatch3.bmp", SIZE, SIZE * 1.333),
        Util::Game::D2::Sprite("/user/dino/player/hatch4.bmp", SIZE, SIZE * 1.333)}), 0.5);
    deathAnimation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/dino/player/death1.bmp", SIZE, SIZE * 1.133),
        Util::Game::D2::Sprite("/user/dino/player/death2.bmp", SIZE, SIZE * 1.133),
        Util::Game::D2::Sprite("/user/dino/player/death3.bmp", SIZE, SIZE * 1.133),
        Util::Game::D2::Sprite("/user/dino/player/death4.bmp", SIZE, SIZE * 1.133),
        Util::Game::D2::Sprite("/user/dino/player/death5.bmp", SIZE, SIZE * 1.133)}), 0.5);

    addComponent(new Util::Game::D2::LinearMovementComponent(*this));
    addComponent(new Util::Game::D2::GravityComponent(*this, 1.25, 0));
}

void PlayerDino::idle() {
    running = false;
}

void PlayerDino::run() {
    running = true;
}

void PlayerDino::jump() {
    if (onGround && hatched && !dying && !dead) {
        setVelocityY(JUMP_VELOCITY);
        onGround = false;
    }
}

void PlayerDino::hatch() {
    if (!hatching && !hatched && !dying && !dead && onGround) {
        currentAnimation = &crackAnimation;
        hatching = true;
    }
}

void PlayerDino::onUpdate(double delta) {
    if (dead) {
        auto &game = Util::Game::GameManager::getGame();
        game.pushScene(new GameOverScreen(points));
        game.switchToNextScene();

        return;
    }

    if (hatching) {
        time += delta;
        if (time >= crackAnimation.getAnimationTime() && time < crackAnimation.getAnimationTime() + hatchAnimation.getAnimationTime()) {
            currentAnimation = &hatchAnimation;
        } else if (time > crackAnimation.getAnimationTime() + hatchAnimation.getAnimationTime()) {
            hatched = true;
            hatching = false;
            currentAnimation = &idleAnimation;
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
    } else if (hatched) {
        if (running) {
            currentAnimation = &runAnimation;

            auto velocityX = getVelocity().getX();
            velocityX += delta * static_cast<int32_t>(direction);
            if (velocityX > MAX_VELOCITY) {
                velocityX = MAX_VELOCITY;
            } else if (velocityX < -MAX_VELOCITY) {
                velocityX = -MAX_VELOCITY;
            }

            setVelocityX(velocityX);
        } else {
            currentAnimation = &idleAnimation;

            auto velocityX = getVelocity().getX();
            auto newVelocityX = velocityX - delta * velocityX > 0 ? -1 : 1;
            if ((velocityX >= 0 && newVelocityX <= 0) || (newVelocityX >= 0 && velocityX <= 0)) {
                newVelocityX = 0;
            }

            setVelocityX(newVelocityX);
        }
    }

    if ((onGround && !dead) || dying) {
        currentAnimation->update(delta);
    }

    getCollider().setSize(currentAnimation->getOriginalSize());
}

void PlayerDino::draw(Util::Game::Graphics &graphics) {
    if (direction == LEFT) {
        currentAnimation->setXFlipped(true);
    } else {
        currentAnimation->setXFlipped(false);
    }

    currentAnimation->draw(graphics, getPosition());

    graphics.setColor(Util::Graphic::Colors::GREEN);
    graphics.drawString(pointsFont, Util::Math::Vector2D(10, 10), Util::String::format("Points: %u", points));
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
        idleAnimation.reset();
        runAnimation.reset();
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
    if (event.getSide() == Util::Game::D2::RectangleCollider::Side::BOTTOM) {
        onGround = true;
    }

    if (event.getCollidedWidth().getTag() == Block::GRASS && event.getSide() == Util::Game::D2::RectangleCollider::BOTTOM && running) {
        auto currentTime = Util::Time::getSystemTime();
        if ((currentTime - lastEmissionTime).toMilliseconds() > EMISSION_INTERVAL_MS) {
            lastEmissionTime = currentTime;
            grassEmitter->emitOnce();
        }
    } else if (event.getCollidedWidth().getTag() == Block::WATER) {
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

void PlayerDino::setDirection(PlayerDino::Direction direction) {
    PlayerDino::direction = direction;
}

PlayerDino::Direction PlayerDino::getDirection() const {
    return direction;
}

uint32_t PlayerDino::getPoints() const {
    return points;
}

void PlayerDino::setPoints(uint32_t points) {
    PlayerDino::points = points;
}

void PlayerDino::addPoints(uint32_t points) {
    PlayerDino::points += points;
}
