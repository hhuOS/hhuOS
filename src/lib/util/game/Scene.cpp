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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "Scene.h"

#include "lib/util/collection/Iterator.h"

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

void Scene::addObject(Entity *object) {
    addList.add(object);
}

void Scene::removeObject(Entity *object) {
    if (!removeList.contains(object)) {
        removeList.add(object);
    }
}

void Scene::applyChanges() {
    for (auto *entity : addList) {
        entity->initialize();
        entities.add(entity);
    }

    for (auto *object : removeList) {
        bool removed;
        do {
            removed = entities.remove(object);
        } while (removed);

        delete object;
    }

    addList.clear();
    removeList.clear();
}

void Scene::draw(Graphics &graphics) {
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

}