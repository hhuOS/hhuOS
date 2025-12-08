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

#ifndef HHUOS_LIB_PULSAR_2D_SCENE_H
#define HHUOS_LIB_PULSAR_2D_SCENE_H

#include "pulsar/Scene.h"

namespace Pulsar {
namespace D2 {

/// Base class for 2D scenes used in the game engine.
/// It extends the base `Pulsar::Scene` class with 2D-specific behaviors.
/// 2D scenes manage 2D entities with rectangle colliders and check for collisions between them.
///
/// To create a new 2D scene for your game, derive from this class and implement the required methods:
/// - `initializeBackground(Graphics &graphics)`: Set up the background for the scene.
/// - `initialize()`: Initialize the scene (load resources, set up entities, etc.).
/// - `update(float delta)`: Update the scene logic (optional).
/// - `keyPressed(const Util::Io::Key &key)`: Handle key press events (optional).
/// - `keyReleased(const Util::Io::Key &key)`: Handle key release events (optional).
/// - `mouseButtonPressed(Util::Io::MouseDecoder::Button button)`: Handle mouse button press events (optional).
/// - `mouseButtonReleased(Util::Io::MouseDecoder::Button button)`: Handle mouse button release events (optional).
/// - `mouseMoved(const Util::Math::Vector2<float> &relativeMovement)`: Handle mouse movement events (optional).
/// - `mouseScrolled(float scrollAmount)`: Handle mouse scroll events (optional).
class Scene : public Pulsar::Scene {

public:
    /// Create a new 2D scene instance.
    Scene() = default;

    /// Initialize the background of the scene.
    /// This method is called as part of the scene initialization process.
    /// Implementations should set up the background graphics here. Everything drawn in this method
    /// is stored in a separate background buffer. This buffer is rendered once at the beginning of each frame,
    /// before any entities are drawn on top of it. The background also scrolls along endlessly with the camera.
    /// If anything has been drawn in this method, it should return `true`.
    /// If no background is needed (i.e. the background is left black), it should return `false`.
    virtual bool initializeBackground(Graphics&) {
        return false;
    }

    /// Initialize the scene. This method is called by the engine once when the scene is first loaded.
    /// It initializes the graphics context, calls the user-defined `initialize()` method and
    /// initializes all entities in the scene.
    /// This implementation calls `initializeBackground()` to set up the background for 2D scenes.
    /// It is not intended to be called directly by game developers.
    void initializeScene(Graphics &graphics) final;

    /// Update all entities in the scene. This method is called by the engine once per frame.
    /// It is not intended to be called directly by game developers.
    void updateEntities(float delta) final;

    /// Check for collisions between entities in the scene. This method is called by the engine once per frame.
    /// It checks for collisions between entities that have rectangle colliders and have moved since the last update.
    /// When a collision is detected, a `CollisionEvent` is created and dispatched to both entities involved in the
    /// collision. It is not intended to be called directly by game developers.
    void checkCollisions() final;
};

}
}

#endif
