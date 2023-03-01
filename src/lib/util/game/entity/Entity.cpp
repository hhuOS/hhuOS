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

#include "lib/util/game/entity/component/Component.h"
#include "lib/util/game/entity/event/CollisionEvent.h"
#include "Entity.h"
#include "lib/util/game/entity/Entity.h"
#include "lib/util/base/Exception.h"
#include "lib/util/game/entity/collider/Collider.h"

namespace Util::Game {

Entity::Entity(uint32_t tag, const Math::Vector2D &position) : tag(tag), position(position), colliderPresent(false), collider(Util::Math::Vector2D(0, 0), Collider::STATIC, 0, 0) {}

Entity::Entity(uint32_t tag, const Math::Vector2D &position, const RectangleCollider &collider) : tag(tag), position(position), colliderPresent(true), collider(collider) {}

void Entity::translate(const Math::Vector2D &translation) {
    velocity = velocity + translation;
}

void Entity::translateX(double x) {
    velocity = Math::Vector2D(x, velocity.getY());
}

void Entity::translateY(double y) {
    position = Math::Vector2D(velocity.getX(), y);
}

void Entity::setPosition(const Math::Vector2D &position) {
    Entity::position = position;
    if (colliderPresent) {
        collider.setPosition(position);
    }

    positionChanged = true;
}

void Entity::setPositionX(double x) {
    position = Math::Vector2D(x, position.getY());
}

void Entity::setPositionY(double y) {
    position = Math::Vector2D(position.getX(), y);
}

void Entity::setVelocity(const Math::Vector2D &velocity) {
    Entity::velocity = velocity;
}

void Entity::setVelocityX(double x) {
    velocity = Math::Vector2D(x, velocity.getY());
}

void Entity::setVelocityY(double y) {
    velocity = Math::Vector2D(velocity.getX(), y);
}

void Entity::addComponent(Component *component) {
    components.add(component);
}

const Util::Math::Vector2D& Entity::getPosition() const {
    return position;
}

const Math::Vector2D &Entity::getVelocity() const {
    return velocity;
}

RectangleCollider& Entity::getCollider() {
    if (!colliderPresent) {
        Exception::throwException(Exception::NULL_POINTER, "Entity: Has no collider!");
    }

    return collider;
}

bool Entity::hasCollider() const {
    return colliderPresent;
}

uint32_t Entity::getTag() const {
    return tag;
}

void Entity::update(double delta) {
    collider.lastPosition = position;
    positionChanged = false;

    for (auto *component : components) {
        component->update(delta);
    }
    
    onUpdate(delta);
}

void Entity::onCollision(CollisionEvent &event) {
    if (collider.getType() == Collider::DYNAMIC) {
        switch (event.getSide()) {
            case RectangleCollider::BOTTOM:
                setPosition(Math::Vector2D(position.getX(), event.getCollidedWidth().getPosition().getY() + event.getCollidedWidth().getCollider().getHeight()));
                if (velocity.getY() < 0) {
                    setVelocityY(0);
                }
                break;
            case RectangleCollider::TOP:
                setPosition(Math::Vector2D(position.getX(), event.getCollidedWidth().getPosition().getY() - collider.getHeight()));
                if (velocity.getY() > 0) {
                    setVelocityY(0);
                }
                break;
            case RectangleCollider::LEFT:
                setPosition(Math::Vector2D(event.getCollidedWidth().getPosition().getX() + event.getCollidedWidth().collider.getWidth(), position.getY()));
                if (velocity.getX() < 0) {
                    setVelocityX(0);
                }
                break;
            case RectangleCollider::RIGHT:
                setPosition(Math::Vector2D(event.getCollidedWidth().getPosition().getX() - collider.getWidth(), position.getY()));
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