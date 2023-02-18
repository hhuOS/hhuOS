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

#ifndef HHUOS_GAME_H
#define HHUOS_GAME_H

#include <cstdint>

#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/Iterator.h"
#include "Camera.h"
#include "lib/util/game/entity/Entity.h"

namespace Util {
namespace Game {
class Graphics2D;
class KeyListener;
class MouseListener;
}  // namespace Game
}  // namespace Util

namespace Util::Game {

class Game {

friend class Engine;

public:
    /**
     * Default Constructor.
     */
    Game() = default;

    /**
     * Copy Constructor.
     */
    Game(const Game &other) = delete;

    /**
     * Assignment operator.
     */
    Game &operator=(const Game &other) = delete;

    /**
     * Destructor.
     */
    virtual ~Game();

    [[nodiscard]] bool isRunning() const;

    [[nodiscard]] uint32_t getObjectCount() const;

    [[nodiscard]] Camera& getCamera();

    void stop();

    void applyChanges();

    void updateEntities(double delta);

    void draw(Graphics2D &graphics);

    virtual void update(double delta) = 0;

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
    Util::ArrayList<Entity*> entities;
    Util::ArrayList<Entity*> addList;
    Util::ArrayList<Entity*> removeList;
    bool running = true;
};

}

#endif
