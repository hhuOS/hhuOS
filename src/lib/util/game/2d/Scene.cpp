/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "Scene.h"

#include "lib/util/collection/Pair.h"
#include "lib/util/game/2d/event/CollisionEvent.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/game/2d/Entity.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/game/Entity.h"
#include "lib/util/game/Graphics.h"

namespace Util::Game::D2 {

void Scene::initializeScene(Graphics &graphics) {
    initializeBackground(graphics);
    graphics.saveCurrentStateAsBackground();

    initialize();

    for (auto *entity : entities) {
        entity->initialize();
    }
}

void Scene::updateEntities(double delta) {
    for (auto *entity : entities) {
        reinterpret_cast<Util::Game::D2::Entity*>(entity)->update(delta);
    }
}

void Scene::checkCollisions() {
    auto detectedCollisions = ArrayList<Pair<Util::Game::D2::Entity*, Util::Game::D2::Entity*>>();
    for (auto *entity : entities) {
        auto *entity2D = reinterpret_cast<Util::Game::D2::Entity*>(entity);
        if (entity2D->hasCollider() && entity2D->positionChanged) {
            const auto &collider = entity2D->getCollider();

            for (auto *otherEntity : entities) {
                auto *otherEntity2D = reinterpret_cast<Util::Game::D2::Entity*>(otherEntity);
                if (entity == otherEntity || !otherEntity2D->hasCollider() || otherEntity2D->isParticle() || detectedCollisions.contains(Pair(entity2D, otherEntity2D)) || detectedCollisions.contains(Pair(otherEntity2D, entity2D))) {
                    continue;
                }

                const auto &otherCollider = otherEntity2D->getCollider();
                auto side = collider.isColliding(otherCollider);

                if (side != RectangleCollider::NONE) {
                    auto event = CollisionEvent(*otherEntity2D, side);
                    auto otherEvent = CollisionEvent(*entity2D, RectangleCollider::getOpposite(side));

                    entity2D->onCollision(event);
                    otherEntity2D->onCollision(otherEvent);

                    detectedCollisions.add(Pair(entity2D, otherEntity2D));
                }
            }
        }
    }
}

}