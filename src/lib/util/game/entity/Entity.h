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
 *
 * The network stack is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-maseh100
 */

#ifndef HHUOS_ENTITY_H
#define HHUOS_ENTITY_H

#include <cstdint>

#include "lib/util/math/Vector2D.h"
#include "lib/util/game/Drawable.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/game/entity/collider/RectangleCollider.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/game/entity/component/Component.h"

namespace Util {
namespace Game {
class TranslationEvent;
}  // namespace Game
}  // namespace Util

namespace Util::Game {

class CollisionEvent;

class Entity : public Drawable {

friend class Scene;

public:
    /**
     * Constructor.
     */
    explicit Entity(uint32_t tag, const Math::Vector2D &position);

    /**
     * Constructor.
     */
    Entity(uint32_t tag, const Math::Vector2D &position, const RectangleCollider &collider);

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

    virtual void initialize() = 0;

    virtual void onUpdate(double delta) = 0;

    virtual void onTranslationEvent(TranslationEvent &event) = 0;

    virtual void onCollisionEvent(CollisionEvent &event) = 0;

    void translate(const Math::Vector2D &translation);

    void translateX(double x);

    void translateY(double y);

    void setPosition(const Math::Vector2D &position);

    void setPositionX(double x);

    void setPositionY(double y);

    void setVelocity(const Math::Vector2D &velocity);

    void setVelocityX(double x);

    void setVelocityY(double y);

    void addComponent(Component *component);

    [[nodiscard]] const Math::Vector2D& getPosition() const;

    [[nodiscard]] const Math::Vector2D& getVelocity() const;

    [[nodiscard]] RectangleCollider& getCollider();

    [[nodiscard]] bool hasCollider() const;

    [[nodiscard]] uint32_t getTag() const;

private:

    void update(double delta);

    void onCollision(CollisionEvent &event);

    uint32_t tag;
    bool positionChanged = false;
    Math::Vector2D position{};
    Math::Vector2D velocity{};

    bool colliderPresent;
    RectangleCollider collider;

    ArrayList<Component*> components;
};

}

#endif
