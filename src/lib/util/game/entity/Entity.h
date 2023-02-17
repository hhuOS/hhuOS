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

#ifndef HHUOS_ENTITY_H
#define HHUOS_ENTITY_H

#include "lib/util/math/Vector2D.h"
#include "lib/util/game/Drawable.h"
#include "lib/util/game/entity/event/TranslationEvent.h"
#include "lib/util/collection/ArrayList.h"

namespace Util::Game {

class Component;

class Entity : public Drawable {

friend class Game;

public:
    /**
     * Default Constructor.
     */
    Entity() = default;

    /**
     * Constructor.
     */
    explicit Entity(const Math::Vector2D &position);

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

    virtual void onUpdate(double delta) = 0;

    virtual void onTranslationEvent(TranslationEvent &event) = 0;

    void translate(const Math::Vector2D &translation);

    void translateX(double x);

    void translateY(double y);

    void setPosition(const Math::Vector2D &position);

    void setVelocity(const Math::Vector2D &velocity);

    void addComponent(Component *component);

    [[nodiscard]] const Math::Vector2D& getPosition() const;

    [[nodiscard]] const Math::Vector2D& getVelocity() const;

private:

    void update(double delta);

    Math::Vector2D position{};
    Math::Vector2D velocity{};

    ArrayList<Component*> components;
};

}

#endif
