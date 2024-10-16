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

#include "EnemyFrog.h"

#include "lib/util/game/2d/event/CollisionEvent.h"
#include "PlayerDino.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/Scene.h"
#include "lib/util/game/2d/component/LinearMovementComponent.h"
#include "lib/util/game/2d/component/GravityComponent.h"
#include "application/dino/particle/BloodEmitter.h"
#include "Block.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/game/2d/Sprite.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/math/Vector2D.h"

namespace Util {
namespace Game {
class Graphics;
namespace D2 {
class TranslationEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

EnemyFrog::EnemyFrog(const Util::Math::Vector2D &position) : Util::Game::D2::Entity(TAG, position, Util::Game::D2::RectangleCollider(position, Util::Math::Vector2D(SIZE, SIZE * 1.12), Util::Game::D2::RectangleCollider::DYNAMIC)) {
    Util::Game::GameManager::getCurrentScene().addObject(grassEmitter);
}

void EnemyFrog::initialize() {
    animation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/dino/enemy/frog1.bmp", SIZE, SIZE * 1.12),
        Util::Game::D2::Sprite("/user/dino/enemy/frog2.bmp", SIZE, SIZE * 1.12),
        Util::Game::D2::Sprite("/user/dino/enemy/frog3.bmp", SIZE, SIZE * 1.12),
        Util::Game::D2::Sprite("/user/dino/enemy/frog4.bmp", SIZE, SIZE * 1.12),
        Util::Game::D2::Sprite("/user/dino/enemy/frog5.bmp", SIZE, SIZE * 1.12),
        Util::Game::D2::Sprite("/user/dino/enemy/frog6.bmp", SIZE, SIZE * 1.12),
        Util::Game::D2::Sprite("/user/dino/enemy/frog7.bmp", SIZE, SIZE * 1.12),
        Util::Game::D2::Sprite("/user/dino/enemy/frog8.bmp", SIZE, SIZE * 1.12),
        Util::Game::D2::Sprite("/user/dino/enemy/frog9.bmp", SIZE, SIZE * 1.12),
        Util::Game::D2::Sprite("/user/dino/enemy/frog10.bmp", SIZE, SIZE * 1.12),
        Util::Game::D2::Sprite("/user/dino/enemy/frog11.bmp", SIZE, SIZE * 1.12),
        Util::Game::D2::Sprite("/user/dino/enemy/frog12.bmp", SIZE, SIZE * 1.12)}), 0.5);

    addComponent(new Util::Game::D2::LinearMovementComponent(*this));
    addComponent(new Util::Game::D2::GravityComponent(*this, 1.25, 0));
    setVelocityX(0.25);
}

void EnemyFrog::onUpdate(double delta) {
    animation.update(delta);
}

void EnemyFrog::draw(Util::Game::Graphics &graphics) {
    animation.draw(graphics, getPosition());
}

void EnemyFrog::onTranslationEvent([[maybe_unused]] Util::Game::D2::TranslationEvent &event) {}

void EnemyFrog::onCollisionEvent(Util::Game::D2::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == BloodEmitter::PARTICLE_TAG) {
        return;
    }

    if (event.getSide() == Util::Game::D2::RectangleCollider::LEFT) {
        setVelocityX(VELOCITY);
        animation.setXFlipped(false);
    } else if (event.getSide() == Util::Game::D2::RectangleCollider::RIGHT) {
        setVelocityX(-VELOCITY);
        animation.setXFlipped(true);
    }

    if (event.getCollidedWidth().getTag() == Block::GRASS && event.getSide() == Util::Game::D2::RectangleCollider::BOTTOM) {
        auto currentTime = Util::Time::getSystemTime();
        if ((currentTime - lastEmissionTime).toMilliseconds() > EMISSION_INTERVAL_MS) {
            lastEmissionTime = currentTime;
            grassEmitter->emitOnce();
        }
    }

    if (event.getCollidedWidth().getTag() == PlayerDino::TAG) {
        auto &player = reinterpret_cast<PlayerDino &>(event.getCollidedWidth());
        auto &scene = Util::Game::GameManager::getGame().getCurrentScene();

        if (player.isDying() || player.isDead()) {
            return;
        }

        if (player.getPosition().getY() >= (getPosition().getY() + SIZE / 2)) {
            player.addPoints(5);
            player.setVelocityY(0.5);
            scene.addObject(new BloodEmitter(getPosition(), BloodEmitter::ENEMY));
            scene.removeObject(this);
        } else {
            player.die();
            scene.addObject(new BloodEmitter(player.getPosition(), BloodEmitter::PLAYER));
        }
    }
}
