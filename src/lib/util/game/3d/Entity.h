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
 */

#ifndef HHUOS_ENTITY_3D_H
#define HHUOS_ENTITY_3D_H

#include "lib/util/game/Entity.h"
#include "lib/util/math/Vector3D.h"

namespace Util::Game::D3 {

class Entity : public Util::Game::Entity {

friend class Scene;

public:
    /**
     * Constructor.
     */
    Entity(uint32_t tag, const Math::Vector3D &position, const Math::Vector3D &rotation, const Math::Vector3D &scale);

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

    virtual void onTransformChange() = 0;

    [[nodiscard]] const Math::Vector3D& getPosition() const;

    void setPosition(const Math::Vector3D &position);

    void translate(const Math::Vector3D &translation);

    void translateLocal(const Math::Vector3D &translation);

    [[nodiscard]] const Math::Vector3D& getRotation() const;

    void setRotation(const Math::Vector3D &rotation);

    void rotate(const Math::Vector3D &rotation);

    [[nodiscard]] const Math::Vector3D& getScale() const;

    void setScale(const Math::Vector3D &scale);

private:

    void update(double delta);

    Math::Vector3D position{};
    Math::Vector3D rotation{};
    Math::Vector3D scale{1, 1, 1};
};

}

#endif
