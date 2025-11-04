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
 */

#ifndef HHUOS_ENTITY_3D_H
#define HHUOS_ENTITY_3D_H

#include "lib/pulsar/Entity.h"

#include <stdint.h>

#include "Orientation.h"
#include "lib/util/math/Vector3.h"
#include "lib/pulsar/3d/collider/SphereCollider.h"

namespace Pulsar::D3 {

class CollisionEvent;

class Entity : public ::Pulsar::Entity {

friend class Scene;

public:
    /**
     * Constructor.
     */
    Entity(uint32_t tag, const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &rotation, const Util::Math::Vector3<double> &scale);

    /**
     * Constructor.
     */
    Entity(uint32_t tag, const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &rotation, const Util::Math::Vector3<double> &scale, const SphereCollider &collider);

    /**
     * Copy Constructor.
     */
    Entity(const Entity &other) = delete;

    /**
     * Assignment operator.
     */
    Entity &operator=(const Entity &other) = delete;

    /**
     * Destructor.
     */
    ~Entity() override = default;

    virtual void onCollisionEvent(CollisionEvent &event) = 0;

    [[nodiscard]] const Util::Math::Vector3<double>& getPosition() const;

    void setPosition(const Util::Math::Vector3<double> &position);

    void translate(const Util::Math::Vector3<double> &translation);

    void translateLocal(const Util::Math::Vector3<double> &translation);

    [[nodiscard]] const Orientation& getOrientation() const;

    [[nodiscard]] const Util::Math::Vector3<double>& getUpVector() const;

    [[nodiscard]] const Util::Math::Vector3<double>& getRightVector() const;

    [[nodiscard]] const Util::Math::Vector3<double>& getFrontVector() const;

    [[nodiscard]] const Util::Math::Vector3<double>& getRotation() const;

    void setFrontVector(const Util::Math::Vector3<double> &front);

    void setRotation(const Util::Math::Vector3<double> &angle);

    void rotate(const Util::Math::Vector3<double> &angle);

    [[nodiscard]] const Util::Math::Vector3<double>& getScale() const;

    void setScale(const Util::Math::Vector3<double> &scale);

    [[nodiscard]] bool hasCollider() const;

    [[nodiscard]] SphereCollider& getCollider();

private:

    void update(double delta);

    Util::Math::Vector3<double> position{};
    Util::Math::Vector3<double> scale{1, 1, 1};
    Orientation orientation;

    bool colliderPresent = false;
    SphereCollider collider;
};

}

#endif
