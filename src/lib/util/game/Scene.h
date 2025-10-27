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

#ifndef HHUOS_LIB_UTIL_SCENE_H
#define HHUOS_LIB_UTIL_SCENE_H

#include <stdint.h>

#include "collection/ArrayList.h"
#include "io/key/Key.h"
#include "io/key/MouseDecoder.h"
#include "game/Camera.h"
#include "game/Graphics.h"

namespace Util {
namespace Game {
class Entity;
}  // namespace Game
}  // namespace Util

namespace Util::Game {

/// Base class for scenes used in the game engine.
/// A scene represents a level within the game, containing its own set of entities, camera, and logic.
/// Scenes manage the lifecycle of their entities, including initialization, updates, rendering, and input handling.
/// A game consists of a sequence of scenes that the player can navigate through. These are managed by the `Game` class.
/// To push a new scene onto the scene stack, use `Game::pushScene()`.
/// To switch to the next scene, use `Game::switchToNextScene()`.
///
/// Internally, a scene keeps three lists of entities:
/// - The main list of active entities. These entities are updated and rendered each frame.
/// - An addition list for entities that are to be added. Entities added via `addEntity()` are placed in this list
///   and moved to the main list at the beginning of the next frame.
/// - A removal list for entities that are to be removed. Entities removed via `removeEntity()` are placed in this list
///   and deleted at the beginning of the next frame.
///
/// This deferred addition and removal mechanism ensures that entities can be safely added or removed during
/// update cycles without causing iterator invalidation or other issues.
///
/// Depending on the type of game (2D or 3D), scenes can have different behaviors, defined in the derived classes
/// `D2::Scene` and `D3::Scene`. Games should always use derive from thes classes instead of this base class directly.
class Scene {

public:
    /// Create a new scene instance.
    Scene() = default;

    /// Scenes are not copyable, since they manage unique resources on the heap,
    /// so the copy constructor is deleted.
    Scene(const Scene &other) = delete;

    /// Scenes are not copyable, since they manage unique resources on the heap,
    /// so the copy assignment operator is deleted.
    Scene &operator=(const Scene &other) = delete;

    /// Destroy the scene and all its entities.
    /// A scene is deleted automatically when the game switches to the next scene.
    virtual ~Scene();

    /// Initialize the scene. This method is called once when the scene is first loaded.
    /// Any scene-wide setup should be done here (e.g. loading resources, setting up the environment, etc.).
    /// All required resources should be loaded here, even if they are not immediately needed,
    /// to avoid loading delays during gameplay.
    virtual void initialize() = 0;

    /// Update the scene. This method is called once per frame with the time delta since the last frame.
    /// All scene-wide logic should be done here. However, entities are updated separately.
    /// Thus, implementing this method is optional, if all logic is handled by entities.
    virtual void update(double delta);

    /// Handle a key press event. Implementing this method is optional.
    virtual void keyPressed(const Io::Key &key);

    /// Handle a key release event. Implementing this method is optional.
    virtual void keyReleased(const Io::Key &key);

    /// Handle a mouse button press event. Implementing this method is optional.
    virtual void mouseButtonPressed(Io::MouseDecoder::Button button);

    /// Handle a mouse button release event. Implementing this method is optional.
    virtual void mouseButtonReleased(Io::MouseDecoder::Button button);

    /// Handle mouse movement events. Implementing this method is optional.
    virtual void mouseMoved(const Math::Vector2<double> &relativeMovement);

    /// Handle mouse scroll events. Implementing this method is optional.
    virtual void mouseScrolled(Io::MouseDecoder::ScrollDirection direction);

    /// Add an entity to the scene. The entity will be initialized and updated automatically.
    /// The scene takes ownership of the entity and will delete it when the scene is destroyed
    /// or the entity is removed.
    /// Adding the same entity multiple times is not allowed and will cause undefined behavior.
    void addEntity(Entity *object);

    /// Remove an entity from the scene. The entity will be deleted automatically.
    /// Removing the same entity multiple times is not allowed and will cause undefined behavior.
    void removeEntity(Entity *object);

    /// Get the camera associated with the scene.
    [[nodiscard]] Camera& getCamera();

protected:

    /// Get const access to the list of entities in the scene. This is mainly used by the `D2::Scene` and `D3::Scene`
    /// implementation to iterate over all entities for rendering and collision detection.
    [[nodiscard]] const ArrayList<Entity*>& getEntities() const;

private:

    friend class Engine;
    friend class Game;

    /// Initialize the scene. This method is called by the engine once when the scene is first loaded.
    /// It initializes the graphics context and calls the user-defined `initialize()` method.
    virtual void initializeScene(Graphics &graphics) = 0;

    /// Update all entities in the scene.
    /// This method is called by the engine once per frame.
    virtual void updateEntities(double delta) = 0;

    /// Check for collisions between entities in the scene.
    /// This method is called by the engine once per frame.
    virtual void checkCollisions() = 0;

    /// Get the current number of entities in the scene.
    /// This method is used by the engine for the statistics overlay.
    [[nodiscard]] uint32_t getObjectCount() const;

    /// Apply any pending additions or removals of entities.
    /// This method is called by the engine at the beginning of each frame.
    void applyChanges();

    /// Draw all entities in the scene.
    /// This method is called by the engine once per frame.
    void draw(Graphics &graphics) const;

    Camera camera;
    ArrayList<Entity*> entities;
    ArrayList<Entity*> addList;
    ArrayList<Entity*> removeList;
};

}

#endif