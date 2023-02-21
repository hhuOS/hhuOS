/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/collection/Pair.h"
#include "Game.h"
#include "lib/util/game/entity/event/CollisionEvent.h"

namespace Util {
namespace Game {
class Graphics2D;
class KeyListener;
class MouseListener;
class Camera;
}  // namespace Game
}  // namespace Util

namespace Util::Game {

Game::~Game() {
    for (const auto *drawable : entities) {
        delete drawable;
    }

    entities.clear();
}

void Game::addObject(Entity *object) {
    addList.add(object);
}

void Game::removeObject(Entity *object) {
    removeList.add(object);
}

void Game::applyChanges() {
    for (auto *entity : addList) {
        entity->initialize();
        entities.add(entity);
    }

    for (auto *object : removeList) {
        entities.remove(object);
        delete object;
    }

    addList.clear();
    removeList.clear();
}

void Game::updateEntities(double delta) {
    for (auto *entity : entities) {
        entity->update(delta);
    }
}

void Game::draw(Graphics2D &graphics) {
    for (auto *object : entities) {
        object->draw(graphics);
    }
}

bool Game::isRunning() const {
    return running;
}

void Game::stop() {
    Game::running = false;
}

uint32_t Game::getObjectCount() const {
    return entities.size();
}

void Game::setKeyListener(KeyListener &listener) {
    keyListener = &listener;
}

void Game::setMouseListener(MouseListener &listener) {
    mouseListener = &listener;
}

Camera& Game::getCamera() {
    return camera;
}

void Game::checkCollisions() {
    auto detectedCollisions = ArrayList<Pair<Entity*, Entity*>>();
    for (auto *entity : entities) {
        if (entity->hasCollider() && entity->positionChanged) {
            const auto &collider = entity->getCollider();

            for (auto *otherEntity : entities) {
                if (entity == otherEntity || !otherEntity->hasCollider() || detectedCollisions.contains(Pair(entity, otherEntity))) {
                    continue;
                }

                const auto &otherCollider = otherEntity->getCollider();
                auto side = collider.isColliding(otherCollider);

                if (side != RectangleCollider::NONE) {
                    auto event = CollisionEvent(*otherEntity, side);
                    auto otherEvent = CollisionEvent(*entity, RectangleCollider::getOpposite(side));

                    entity->onCollision(event);
                    otherEntity->onCollision(otherEvent);
                    detectedCollisions.add(Pair(entity, otherEntity));
                }
            }
        }
    }
}

void Game::initialize(Graphics2D &graphics) {
    initializeBackground(graphics);
    graphics.saveCurrentStateAsBackground();
}

}