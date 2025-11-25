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

#ifndef HHUOS_LIB_PULSAR_2D_ENTITY_H
#define HHUOS_LIB_PULSAR_2D_ENTITY_H

#include <stddef.h>

#include "util/collection/ArrayList.h"
#include "util/math/Vector2.h"
#include "pulsar/Entity.h"
#include "pulsar/2d/collider/RectangleCollider.h"

namespace Pulsar {
namespace D2 {
class CollisionEvent;
class TranslationEvent;
class Component;
}  // namespace D2
}  // namespace Pulsar

namespace Pulsar::D2 {

/// Base class for 2D entities for use in 2D scenes.
/// It enhances the `Pulsar::Entity` class with 2D-specific properties like position, velocity and a collider.
/// Furthermore, it introduces a component system, allowing to attach reusable components to entities
/// that can modify their behavior (e.g. movement). For example, a `GravityComponent` can be added to an entity
/// to simulate an acceleration due to gravity. If a `LinearMovementComponent` is added, the entity will move
/// linearly based on its velocity. Without this component, the entity will remain stationary unless its position
/// is modified directly, regardless of its velocity.
/// To handle collisions, 2D entities can have a `RectangleCollider`. If an entity has a collider and collides
/// with another entity that also has a collider, a `CollisionEvent` is triggered,
/// allowing to respond to the collision (e.g. by bouncing off or taking damage).
class Entity : public Pulsar::Entity {

public:

    /// Create a new 2D entity with the given tag, position and rectangle collider.
    /// The collider is optional and defaults to a collider of type `NONE`.
    Entity(size_t tag, const Util::Math::Vector2<float> &position,
        const RectangleCollider &collider = RectangleCollider());

    /// 2D entities are not copyable, as they manage components on the heap, so the copy constructor is deleted.
    Entity (const Entity &other) = delete;

    /// 2D entities are not copyable, as they manage components on the heap, so the assignment operator is deleted.
    Entity& operator=(const Entity &other) = delete;

    /// Destroy the 2D entity and clean up all its components.
    ~Entity() override;

    /// This method is called whenever the entity is moved by a component (e.g. `LinearMovementComponent`),
    /// or via the `translate()` method. It is not called when the position is set directly via `setPosition()`.
    /// The entity may cancel the translation by calling `event.cancel()`, preventing the movement.
    /// The default implementation does nothing.
    virtual void onTranslationEvent(TranslationEvent &event);

    /// This method is called whenever the entity collides with another entity that has a collider.
    /// The default implementation does nothing.
    virtual void onCollisionEvent(const CollisionEvent &event);

    /// Get the current position of the entity.
    [[nodiscard]] const Util::Math::Vector2<float>& getPosition() const;

    /// Set the absolute position of the entity.
    /// This will NOT trigger a translation event, the entity will be moved directly.
    void setPosition(const Util::Math::Vector2<float> &position);

    /// Set the absolute x-coordinate of the entity.
    /// This will NOT trigger a translation event, the entity will be moved directly.
    void setPositionX(float x);

    /// Set the absolute y-coordinate of the entity.
    /// This will NOT trigger a translation event, the entity will be moved directly.
    void setPositionY(float y);

    /// Move the entity relative to its current position by the given translation vector.
    /// This will trigger a translation event and the entity may cancel the movement.
    void translate(const Util::Math::Vector2<float> &translation);

    /// Move the entity relative to its current position by the given x offset.
    /// This will trigger a translation event and the entity may cancel the movement.
    void translateX(float x);

    /// Move the entity relative to its current position by the given y offset.
    /// This will trigger a translation event and the entity may cancel the movement.
    void translateY(float y);

    /// Get the current velocity of the entity.
    [[nodiscard]] const Util::Math::Vector2<float>& getVelocity() const;

    /// Set the velocity of the entity.
    /// The velocity only affects the entity if a movement component (e.g. `LinearMovementComponent`) is attached to it.
    void setVelocity(const Util::Math::Vector2<float> &velocity);

    /// Set the x-component of the entity's velocity.
    /// The velocity only affects the entity if a movement component (e.g. `LinearMovementComponent`) is attached to it.
    void setVelocityX(float x);

    /// Set the y-component of the entity's velocity.
    /// The velocity only affects the entity if a movement component (e.g. `LinearMovementComponent`) is attached to it.
    void setVelocityY(float y);

    /// Check if the entity has a rectangle collider.
    [[nodiscard]] bool hasCollider() const;

    /// Get the rectangle collider of the entity for modification.
    /// If the entity has no collider, a panic is fired.
    [[nodiscard]] RectangleCollider& getCollider();

    /// Set the rectangle collider of the entity.
    void setCollider(const RectangleCollider &collider);

    /// Add a component to the entity.
    /// The component must be heap-allocated and the entity will take ownership of it and delete it upon destruction.
    void addComponent(Component *component);

    /// Update the entity and all its components.
    /// This method is called automatically every frame with the time delta since the last frame.
    /// It updates the collider's position and then calls the `update()` method of all attached components.
    /// Lastly, it calls the `onUpdate()` method for any additional custom update logic.
    void update(float delta);

    /// Handle a collision with another entity.
    /// This method is called by the scene when a collision is detected between this entity and another entity.
    /// If the entity has a dynamic collider, it will respond to the collision by adjusting its position and velocity.
    /// Furthermore, it calls the `onCollisionEvent()` method to allow custom collision handling.
    void onCollision(const CollisionEvent &event);

    /// Check if the entity's position has changed since the last update.
    /// This method is used by the scene to determine if the entity needs to be re-evaluated for collisions.
    [[nodiscard]] bool hasPositionChanged() const;

private:

    bool positionChanged = false;
    Util::Math::Vector2<float> position{};
    Util::Math::Vector2<float> velocity{};

    RectangleCollider collider;

    Util::ArrayList<Component*> components;
};

}

#endif
