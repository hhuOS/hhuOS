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
 */

#include "lib/util/game/2d/component/Component.h"
#include "lib/util/game/2d/event/CollisionEvent.h"
#include "lib/util/game/2d/Entity.h"
#include "lib/util/base/Panic.h"
#include "lib/util/game/2d/event/TranslationEvent.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/game/Collider.h"

namespace Util::Game::D2 {

Entity::Entity(uint32_t tag, const Math::Vector2<double> &position) : Util::Game::Entity(tag), position(position), colliderPresent(false), collider(Util::Math::Vector2<double>(0, 0), Util::Math::Vector2<double>(0, 0), Collider::STATIC) {}

Entity::Entity(uint32_t tag, const Math::Vector2<double> &position, const RectangleCollider &collider) : Util::Game::Entity(tag), position(position), colliderPresent(true), collider(collider) {}

Entity::~Entity() {
    for (auto *component : components) {
        delete component;
    }
}

void Entity::translate(const Math::Vector2<double> &translation) {
    auto newPosition = position + translation;
    auto event = TranslationEvent(newPosition);
    onTranslationEvent(event);

    if (!event.isCanceled()) {
        setPosition(newPosition);
    }
}

void Entity::translateX(double x) {
    translate(Math::Vector2<double>(x, 0));
}

void Entity::translateY(double y) {
    translate(Math::Vector2<double>(0, y));
}

void Entity::setPosition(const Math::Vector2<double> &position) {
    Entity::position = position;
    if (colliderPresent) {
        collider.setPosition(position);
    }

    positionChanged = true;
}

void Entity::setPositionX(double x) {
    position = Math::Vector2<double>(x, position.getY());
}

void Entity::setPositionY(double y) {
    position = Math::Vector2<double>(position.getX(), y);
}

void Entity::setVelocity(const Math::Vector2<double> &velocity) {
    Entity::velocity = velocity;
}

void Entity::setVelocityX(double x) {
    velocity = Math::Vector2<double>(x, velocity.getY());
}

void Entity::setVelocityY(double y) {
    velocity = Math::Vector2<double>(velocity.getX(), y);
}

void Entity::setCollider(const RectangleCollider &collider) {
    Entity::collider = collider;
    colliderPresent = true;
}

void Entity::addComponent(Component *component) {
    components.add(component);
}

const Util::Math::Vector2<double>& Entity::getPosition() const {
    return position;
}

const Math::Vector2<double> &Entity::getVelocity() const {
    return velocity;
}

RectangleCollider& Entity::getCollider() {
    if (!colliderPresent) {
        Panic::fire(Panic::NULL_POINTER, "Entity: Has no collider!");
    }

    return collider;
}

bool Entity::hasCollider() const {
    return colliderPresent;
}

void Entity::update(double delta) {
    collider.setPosition(Util::Math::Vector2<double>(position.getX(), position.getY()));
    positionChanged = false;

    for (auto *component : components) {
        component->update(delta);
    }
    
    onUpdate(delta);
}

void Entity::onCollision(CollisionEvent &event) {
    if (collider.getType() == Collider::DYNAMIC && event.getCollidedWidth().getCollider().getType() != Collider::PERMEABLE) {
        switch (event.getSide()) {
            case RectangleCollider::BOTTOM:
                setPosition(Math::Vector2<double>(position.getX(), event.getCollidedWidth().getPosition().getY() + event.getCollidedWidth().getCollider().getHeight()));
                if (velocity.getY() < 0) {
                    setVelocityY(0);
                }
                break;
            case RectangleCollider::TOP:
                setPosition(Math::Vector2<double>(position.getX(), event.getCollidedWidth().getPosition().getY() - collider.getHeight()));
                if (velocity.getY() > 0) {
                    setVelocityY(0);
                }
                break;
            case RectangleCollider::LEFT:
                setPosition(Math::Vector2<double>(event.getCollidedWidth().getPosition().getX() + event.getCollidedWidth().collider.getWidth(), position.getY()));
                if (velocity.getX() < 0) {
                    setVelocityX(0);
                }
                break;
            case RectangleCollider::RIGHT:
                setPosition(Math::Vector2<double>(event.getCollidedWidth().getPosition().getX() - collider.getWidth(), position.getY()));
                if (velocity.getX() > 0) {
                    setVelocityX(0);
                }
                break;
            default:
                break;
        }
    }

    onCollisionEvent(event);
}

}