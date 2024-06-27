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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "Scene.h"

#include "Entity.h"
#include "lib/util/collection/Pair.h"
#include "lib/util/game/3d/event/CollisionEvent.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/game/3d/collider/SphereCollider.h"
#include "lib/util/game/Entity.h"
#include "lib/util/game/Graphics.h"

namespace Util::Game::D3 {

void Scene::initialize(Graphics &graphics) {
    graphics.clear();

    for (auto *entity : entities) {
        entity->initialize();
    }
}

void Scene::updateEntities(double delta) {
    for (auto *entity : entities) {
        reinterpret_cast<Util::Game::D3::Entity*>(entity)->update(delta);
    }
}

void Scene::checkCollisions() {
    auto detectedCollisions = Util::ArrayList<Pair<Entity*, Entity*>>();

    for (auto *entity : entities) {
        auto *entity3D = reinterpret_cast<D3::Entity*>(entity);

        if (entity3D->hasCollider()) {
            for (auto *otherEntity : entities) {
                auto *otherEntity3D = reinterpret_cast<D3::Entity*>(otherEntity);
                if (entity == otherEntity || !otherEntity3D->hasCollider() || detectedCollisions.contains(Util::Pair(entity3D, otherEntity3D)) || detectedCollisions.contains(Util::Pair(otherEntity3D, entity3D))) {
                    continue;
                }

                if (entity3D->getCollider().isColliding(otherEntity3D->getCollider())) {
                    auto event = CollisionEvent(*otherEntity3D);
                    auto otherEvent = CollisionEvent(*entity3D);

                    entity3D->onCollisionEvent(event);
                    otherEntity3D->onCollisionEvent(otherEvent);

                    detectedCollisions.add(Util::Pair(entity3D, otherEntity3D));
                }
            }
        }
    }

}

}