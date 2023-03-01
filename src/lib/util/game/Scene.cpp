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
 *
 * The network stack is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-maseh100
 */

#include "Scene.h"

#include "lib/util/game/entity/event/CollisionEvent.h"
#include "lib/util/collection/Pair.h"
#include "lib/util/game/Graphics2D.h"
#include "lib/util/game/entity/collider/RectangleCollider.h"

namespace Util {
namespace Game {
class Camera;
class KeyListener;
class MouseListener;
}  // namespace Game
}  // namespace Util

namespace Util::Game {

Scene::~Scene() {
    applyChanges();

    for (const auto *entity : entities) {
        delete entity;
    }

    entities.clear();
}

void Scene::initialize(Graphics2D &graphics) {
    initializeBackground(graphics);
    graphics.saveCurrentStateAsBackground();

    for (auto *entity : entities) {
        entity->initialize();
    }
}

void Scene::addObject(Entity *object) {
    addList.add(object);
}

void Scene::removeObject(Entity *object) {
    removeList.add(object);
}

void Scene::applyChanges() {
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

void Scene::updateEntities(double delta) {
    for (auto *entity : entities) {
        entity->update(delta);
    }
}

void Scene::draw(Graphics2D &graphics) {
    for (auto *object : entities) {
        object->draw(graphics);
    }
}

uint32_t Scene::getObjectCount() const {
    return entities.size();
}

void Scene::setKeyListener(KeyListener &listener) {
    keyListener = &listener;
}

void Scene::setMouseListener(MouseListener &listener) {
    mouseListener = &listener;
}

Camera& Scene::getCamera() {
    return camera;
}

void Scene::checkCollisions() {
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

}