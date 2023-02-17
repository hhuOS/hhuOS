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

Dino::Dino() : Util::Game::Entity(Util::Math::Vector2D(-0.1, -0.1)), currentAnimation(&eggAnimation),
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
    if (hatched && !isDying) {
        setVelocity({-0.1, 0});
        currentAnimation = dashing ? &dashAnimation : &runAnimation;
        invert = true;
    }
}

void Dino::moveRight() {
    if (hatched && !isDying) {
        setVelocity({0.1, 0});
        currentAnimation = dashing ? &dashAnimation : &runAnimation;
        invert = false;
    }
}

void Dino::dash(bool dash) {
    if (hatched && !isDying && dashing != dash) {
        setVelocity(getVelocity() * (dash ? 2 : 0.5));
        dashing = dash;
    }
}

void Dino::stop() {
    if (hatched && !isDying) {
        setVelocity({0, 0});
        currentAnimation = &idleAnimation;
        runAnimation.reset();
        dashAnimation.reset();
    }
}

void Dino::hatch() {
    if (!isHatching && !hatched && !isDying) {
        currentAnimation = &crackAnimation;
        isHatching = true;
    }
}

void Dino::onUpdate(double delta) {
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
            reset();
        }
    }

    currentAnimation->update(delta);
}

void Dino::onTranslationEvent(Util::Game::TranslationEvent &event) {
    if (!hatched || isDying) {
        event.cancel();
    }
}

void Dino::draw(Util::Game::Graphics2D &graphics) const {
    graphics.drawImage(getPosition(), currentAnimation->getCurrentSprite().getImage(), invert);
}

void Dino::die() {
    if (hatched && !isDying) {
        time = 0;
        isDying = true;
        currentAnimation = &deathAnimation;
    }
}

void Dino::reset() {
    if (!isHatching) {
        time = 0;
        hatched = false;
        isHatching = false;
        isDying = false;
        idleAnimation.reset();
        runAnimation.reset();
        dashAnimation.reset();
        eggAnimation.reset();
        crackAnimation.reset();
        hatchAnimation.reset();
        deathAnimation.reset();
        currentAnimation = &eggAnimation;
    }
}
