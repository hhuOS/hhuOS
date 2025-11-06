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

#ifndef HHUOS_LIB_PULSAR_2D_COMPONENT_H
#define HHUOS_LIB_PULSAR_2D_COMPONENT_H

namespace Pulsar {
namespace D2 {
class Entity;
}  // namespace D2
}  // namespace Pulsar

namespace Pulsar::D2 {

/// A component that can be attached to an entity to extend its functionality.
/// Components are updated every frame by the entity they are attached to.
/// They can access the entity they are attached to via `getEntity()` and modify its state.
/// For example, a `LinearMovementComponent` updates the position of the entity based on its velocity.
/// The entity takes ownership of the component and is responsible for its lifetime.
class Component {

public:
    /// Create a new component instance.
    explicit Component() = default;

    /// Components are not copyable, since they are owned by a single entity, so the copy constructor is deleted.
    Component(const Component &other) = delete;

    /// Components are not copyable, since they are owned by a single entity, so the assignment operator is deleted.
    Component &operator=(const Component &other) = delete;

    /// Destroy the component. Since the base class does not manage any resources, the default destructor is sufficient.
    virtual ~Component() = default;

protected:
    /// This method is called every frame by the entity it is attached to.
    /// Here, the component can modify the state of the entity.
    virtual void update(double delta) = 0;

    /// Get the entity this component is attached to.
    /// This method allows subclasses to access the entity they are attached to.
    [[nodiscard]] Entity& getEntity() const;

private:

    friend class Entity;

    Entity *entity = nullptr;
};

}

#endif
