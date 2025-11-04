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

#ifndef HHUOS_LIB_UTIL_GAME_ENTITY_H
#define HHUOS_LIB_UTIL_GAME_ENTITY_H

#include <stddef.h>

#include "pulsar/Scene.h"
#include "pulsar/Drawable.h"
#include "pulsar/Graphics.h"

namespace Pulsar {

/// Base class for all entities in the game engine.
/// Entities represent objects in the game world that can be drawn and updated (e.g. players, enemies, items, etc.).
/// Each entity has a tag that can be used to identify it. The tag can be unique, but it is not enforced by the engine.
/// It can also be used to identify groups of entities (e.g. all entities with tag 1 are players,
/// tag 2 are enemies, tag 3 are items, etc.).
/// Depending on the type of game (2D or 3D), entities can have different properties and behaviors,
/// defined in the derived classes `D2::Entity` and `D3::Entity`. Games should always use these derived classes
/// instead of this base class directly.
/// To add a new entity to the current scene, use `Scene::addObject(Entity*)`. The entity will then be
/// initialized, updated and drawn automatically by the engine. The given entity instance must be heap-allocated
/// and the scene will take ownership of it (i.e. it will be deleted automatically, when the scene is destroyed
/// or the entity is removed from the scene).
class Entity : public Drawable {

public:
    /// Create a new entity instance with the given tag.
    explicit Entity(size_t tag);

    /// Initialize the entity. This method is called once when the entity is added to the scene.
    /// If the entity needs resources to be loaded or other setup to be done, it should be done here
    /// (e.g. loading sprites, textures, models, or setting initial states).
    /// All required resources should be loaded here, even if they are not immediately needed,
    /// to avoid loading delays during gameplay. For example, if an entity has multiple animations,
    /// all animation frames should be loaded in this method.
    virtual void initialize() = 0;

    /// Update the entity. This method is called once per frame with the time delta since the last update.
    /// The entity should update its state based on the elapsed time (e.g. position, animation, behavior, etc.).
    /// Ignoring the delta time can lead to inconsistent behavior with varying frame rates.
    /// The elapsed time is given in seconds.
    virtual void onUpdate(double delta) = 0;

    /// Draw the entity using the given graphics context.
    /// This method is called once per frame after all entities have been updated.
    /// Some entities may choose not to draw anything (e.g. invisible entities like particle emitters),
    /// in which case they do not need to override this method.
    void draw(Graphics &graphics) const override;

    /// Get the tag of the entity.
    [[nodiscard]] size_t getTag() const;

    /// Get the scene the entity belongs to.
    /// If the entity has not been added to a scene yet, this method will fire a panic.
    [[nodiscard]] Scene& getScene() const;

    /// Remove the entity from its scene.
    /// The entity will be deleted automatically, when the scene is next updated.
    /// If the entity is not part of a scene, this method will fire a panic.
    void removeFromScene();

private:

    friend class Scene;

    size_t tag;

    Scene *scene = nullptr;
};

}

#endif
