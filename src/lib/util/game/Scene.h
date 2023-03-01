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

#ifndef HHUOS_SCENE_H
#define HHUOS_SCENE_H

#include <cstdint>

#include "Camera.h"
#include "lib/util/game/entity/Entity.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/Iterator.h"

namespace Util {
namespace Game {
class Graphics2D;
class KeyListener;
class MouseListener;
}  // namespace Game
}  // namespace Util

namespace Util::Game {

class Scene {

friend class Engine;

public:
    /**
     * Default Constructor.
     */
    Scene() = default;

    /**
     * Copy Constructor.
     */
    Scene(const Scene &other) = delete;

    /**
     * Assignment operator.
     */
    Scene &operator=(const Scene &other) = delete;

    /**
     * Destructor.
     */
    virtual ~Scene();

    void initialize(Graphics2D &graphics);

    [[nodiscard]] uint32_t getObjectCount() const;

    [[nodiscard]] Camera& getCamera();

    void applyChanges();

    void updateEntities(double delta);

    void draw(Graphics2D &graphics);

    virtual void update(double delta) = 0;

    virtual void initializeBackground(Graphics2D &graphics) = 0;

protected:

    void addObject(Entity *object);

    void removeObject(Entity *object);

    void setKeyListener(KeyListener &listener);

    void setMouseListener(MouseListener &listener);

private:

    void checkCollisions();

    KeyListener *keyListener = nullptr;
    MouseListener *mouseListener = nullptr;

    Camera camera;
    ArrayList<Entity*> entities;
    ArrayList<Entity*> addList;
    ArrayList<Entity*> removeList;
};

}

#endif