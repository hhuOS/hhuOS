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

#include "util/base/Panic.h"
#include "pulsar/2d/Entity.h"
#include "pulsar/2d/component/Component.h"
#include "pulsar/2d/event/CollisionEvent.h"
#include "pulsar/2d/event/TranslationEvent.h"

namespace Pulsar {
namespace D2 {

void Entity::translate(const Util::Math::Vector2<float> &translation) {
    const auto newPosition = position + translation;
    auto event = TranslationEvent(newPosition);
    onTranslationEvent(event);

    if (!event.isCanceled()) {
        setPosition(newPosition);
    }
}

void Entity::setPosition(const Util::Math::Vector2<float> &position) {
    Entity::position = position;
    if (hasCollider()) {
        collider.setPosition(position);
    }

    positionChanged = true;
}

void Entity::addComponent(Component *component) {
    component->entity = this;
    components.add(component);
}

void Entity::update(const float delta) {
    collider.setPosition(Util::Math::Vector2<float>(position.getX(), position.getY()));
    positionChanged = false;

    for (auto *component : components) {
        component->update(delta);
    }
    
    onUpdate(delta);
}

void Entity::onCollision(const CollisionEvent &event) {
    const auto posX = position.getX();
    const auto posY = position.getY();
    const auto height = collider.getHeight();
    const auto width = collider.getWidth();
    const auto velX = velocity.getX();
    const auto velY = velocity.getY();

    const auto &otherCollider = event.getCollidedWidth().getCollider();
    const auto &otherPosition = event.getCollidedWidth().getPosition();
    const auto otherPosX = otherPosition.getX();
    const auto otherPosY = otherPosition.getY();
    const auto otherHeight = otherCollider.getHeight();
    const auto otherWidth = otherCollider.getWidth();
    
    if (collider.getType() == RectangleCollider::DYNAMIC && otherCollider.getType() != RectangleCollider::PERMEABLE) {
        switch (event.getSide()) {
            case RectangleCollider::BOTTOM:
                setPosition(Util::Math::Vector2<float>(posX, otherPosY + otherHeight));
                if (velY < 0) {
                    setVelocityY(0);
                }
                break;
            case RectangleCollider::TOP:
                setPosition(Util::Math::Vector2<float>(posX, otherPosY - height));
                if (velY > 0) {
                    setVelocityY(0);
                }
                break;
            case RectangleCollider::LEFT:
                setPosition(Util::Math::Vector2<float>(otherPosX + otherWidth, posY));
                if (velX < 0) {
                    setVelocityX(0);
                }
                break;
            case RectangleCollider::RIGHT:
                setPosition(Util::Math::Vector2<float>(otherPosX - width, posY));
                if (velX > 0) {
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
}
