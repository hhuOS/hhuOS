/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_ENTITY_2D_H
#define HHUOS_ENTITY_2D_H

#include <stdint.h>

#include "lib/util/math/Vector2.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/game/Entity.h"

namespace Util {
namespace Game {
namespace D2 {
class TranslationEvent;
class Component;
}  // namespace D2
}  // namespace Game
}  // namespace Util

namespace Util::Game::D2 {

class CollisionEvent;

class Entity : public Util::Game::Entity {

friend class Scene;

public:
    /**
     * Constructor.
     */
    explicit Entity(uint32_t tag, const Math::Vector2<double> &position);

    /**
     * Constructor.
     */
    Entity(uint32_t tag, const Math::Vector2<double> &position, const RectangleCollider &collider);

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
    ~Entity() override;

    virtual void onTranslationEvent(TranslationEvent &event) = 0;

    virtual void onCollisionEvent(CollisionEvent &event) = 0;

    void translate(const Math::Vector2<double> &translation);

    void translateX(double x);

    void translateY(double y);

    void setPosition(const Math::Vector2<double> &position);

    void setPositionX(double x);

    void setPositionY(double y);

    void setVelocity(const Math::Vector2<double> &velocity);

    void setVelocityX(double x);

    void setVelocityY(double y);

    void setCollider(const RectangleCollider &collider);

    void addComponent(Component *component);

    [[nodiscard]] const Math::Vector2<double>& getPosition() const;

    [[nodiscard]] const Math::Vector2<double>& getVelocity() const;

    [[nodiscard]] bool hasCollider() const;

    [[nodiscard]] RectangleCollider& getCollider();

private:

    void update(double delta);

    void onCollision(CollisionEvent &event);

    bool positionChanged = false;
    Math::Vector2<double> position{};
    Math::Vector2<double> velocity{};

    bool colliderPresent;
    RectangleCollider collider;

    ArrayList<Component*> components;
};

}

#endif
