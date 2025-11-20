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

#ifndef HHUOS_LIB_PULSAR_3D_ENTITY_H
#define HHUOS_LIB_PULSAR_3D_ENTITY_H

#include <stddef.h>

#include "util/math/Vector3.h"
#include "pulsar/Entity.h"
#include "pulsar/3d/Orientation.h"
#include "pulsar/3d/collider/SphereCollider.h"

namespace Pulsar {
namespace D3 {
class CollisionEvent;
}  // namespace D3
}  // namespace Pulsar

namespace Pulsar::D3 {

/// Base class for 3D entities for use in 3D scenes.
/// It enhances the `Pulsar::Entity` class with 3D-specific properties like position, rotation, scale and a collider.
/// To handle collisions, 3D entities can have a `SphereCollider`. If an entity has a collider and collides
/// with another entity that also has a collider, a `CollisionEvent` is triggered,
/// allowing the entity to respond allowing to respond to the collision (e.g. by taking damage).
class Entity : public Pulsar::Entity {

public:
    /// Create a new 3D entity with the given tag, position, rotation, scale and sphere collider.
    /// The collider is optional and defaults to a collider with radius 0 (no collider).
    Entity(size_t tag, const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &rotation,
        const Util::Math::Vector3<double> &scale, const SphereCollider &collider = SphereCollider());

    /// This method is called whenever the entity collides with another entity that has a collider.
    /// The default implementation does nothing.
    virtual void onCollisionEvent(const CollisionEvent &event);

    /// Get the current position of the entity in 3D space.
    [[nodiscard]] const Util::Math::Vector3<double>& getPosition() const;

    /// Set the position of the entity in 3D space.
    void setPosition(const Util::Math::Vector3<double> &position);

    /// Translate the entity by the given translation vector in world space,
    /// regardless of the entity's current orientation.
    void translate(const Util::Math::Vector3<double> &translation);

    /// Translate the entity by the given translation vector in local space,
    /// taking into account the entity's current orientation.
    /// The given translation vector is rotated by the entity's rotation before applying it.
    void translateLocal(const Util::Math::Vector3<double> &translation);

    /// Get the current orientation of the entity.
    [[nodiscard]] const Orientation& getOrientation() const;

    /// Get the current upward pointing vector of the entity.
    [[nodiscard]] const Util::Math::Vector3<double>& getUpVector() const;

    /// Get the current right pointing vector of the entity.
    [[nodiscard]] const Util::Math::Vector3<double>& getRightVector() const;

    /// Get the current front pointing vector of the entity.
    [[nodiscard]] const Util::Math::Vector3<double>& getFrontVector() const;

    /// Set the front vector of the entity, adjusting its orientation accordingly.
    /// This rotates the entity so that its front vector matches the given vector.
    void setFrontVector(const Util::Math::Vector3<double> &front);

    /// Get the current rotation of the entity in degrees.
    [[nodiscard]] const Util::Math::Vector3<double>& getRotation() const;

    /// Set the rotation of the entity in degrees.
    void setRotation(const Util::Math::Vector3<double> &angle);

    /// Rotate the entity by the given angle in degrees.
    void rotate(const Util::Math::Vector3<double> &angle);

    /// Get the current scale of the entity.
    [[nodiscard]] const Util::Math::Vector3<double>& getScale() const;

    /// Set the scale of the entity.
    void setScale(const Util::Math::Vector3<double> &scale);

    /// Check if the entity has a collider.
    [[nodiscard]] bool hasCollider() const;

    /// Get the sphere collider of the entity.
    /// If the entity has no collider, a sphere collider with radius 0 is returned.
    [[nodiscard]] SphereCollider& getCollider();

    /// Update the entity.
    /// This method is called automatically every frame with the time delta since the last frame.
    /// It updates the collider's position and then calls the `update()` method of all attached components.
    /// Lastly, it calls the `onUpdate()` method for any additional custom update logic.
    void update(double delta);

private:

    Util::Math::Vector3<double> position;
    Util::Math::Vector3<double> scale = Util::Math::Vector3<double>(1, 1, 1);
    Orientation orientation;

    SphereCollider collider;
};

}

#endif
