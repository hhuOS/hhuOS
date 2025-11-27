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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#include "Scene.h"

#include "lib/util/collection/Pair.h"
#include "lib/pulsar/2d/event/CollisionEvent.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/pulsar/2d/Entity.h"
#include "lib/pulsar/2d/collider/RectangleCollider.h"
#include "lib/pulsar/Entity.h"
#include "lib/pulsar/Graphics.h"

namespace Pulsar::D2 {

bool Scene::initializeBackground([[maybe_unused]] Graphics &graphics) {
    return false;
}

void Scene::initializeScene(Graphics &graphics) {
    if (initializeBackground(graphics)) {
        graphics.saveCurrentStateAsBackground();
    }

    initialize();

    for (auto *entity : getEntities()) {
        entity->initialize();
    }
}

void Scene::updateEntities(const float delta) {
    for (auto *entity : getEntities()) {
        reinterpret_cast<Entity*>(entity)->update(delta);
    }
}

void Scene::checkCollisions() {
    Util::ArrayList<Util::Pair<Entity*, Entity*>> detectedCollisions;
    for (auto *entity : getEntities()) {
        auto *entity2D = reinterpret_cast<Entity*>(entity);

        // Only check for collisions if the entity has a collider and its position has changed since the last update
        if (entity2D->hasCollider() && entity2D->hasPositionChanged()) {
            const auto &collider = entity2D->getCollider();

            // Check the current entity against all other entities
            for (auto *otherEntity : getEntities()) {
                auto *otherEntity2D = reinterpret_cast<Entity*>(otherEntity);

                // Skip check if it's the same entity, if the other entity has no collider,
                // or if the collision has already been detected in this update cycle
                if (entity == otherEntity ||
                    !otherEntity2D->hasCollider() ||
                    detectedCollisions.contains(Util::Pair<Entity*, Entity*>(entity2D, otherEntity2D)) ||
                    detectedCollisions.contains(Util::Pair<Entity*, Entity*>(otherEntity2D, entity2D)))
                {
                    continue;
                }

                const auto &otherCollider = otherEntity2D->getCollider();
                const auto side = collider.isColliding(otherCollider);

                if (side != RectangleCollider::NONE) {
                    const auto event = CollisionEvent(*otherEntity2D, side);
                    const auto otherEvent = CollisionEvent(*entity2D, RectangleCollider::getOpposite(side));

                    entity2D->onCollision(event);
                    otherEntity2D->onCollision(otherEvent);

                    detectedCollisions.add(Util::Pair<Entity*, Entity*>(entity2D, otherEntity2D));
                }
            }
        }
    }
}

}