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

#include "pulsar/Entity.h"

namespace Pulsar {

Scene::~Scene() {
    applyChanges();

    for (const auto *entity : entities) {
        delete entity;
    }

    entities.clear();
}

void Scene::addEntity(Entity *object) {
    object->scene = this;
    addList.add(object);
}

void Scene::removeEntity(const Entity *object) {
    if (!removeList.contains(object)) {
        removeList.add(object);
    }
}

void Scene::applyChanges() {
    // An entity might add further entities during its initialization, so we need to loop until all are added.
    while (addList.size() > 0) {
        // We copy the list to avoid issues with entities adding further entities during initialization.
        for (auto *entity: addList.toArray()) {
            entity->initialize();
            entities.add(entity);
            addList.remove(entity);
        }
    }

    // An entity might remove further entities during its deletion, so we need to loop until all are removed.
    while (removeList.size() > 0) {
        // We copy the list to avoid issues with entities removing further entities during deletion.
        for (const auto *object: removeList.toArray()) {
            entities.remove(const_cast<Entity*>(object));
            removeList.remove(object);
            delete object;
        }
    }
}

void Scene::draw(Graphics &graphics) const {
    for (const auto *object : entities) {
        object->draw(graphics);
    }
}

}