/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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
 * The rogue game has been implemented during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#include "lib/util/math/Math.h"
#include "lib/pulsar/Scene.h"
#include "lib/pulsar/Graphics.h"
#include "lib/pulsar/3d/collider/SphereCollider.h"
#include "lib/pulsar/3d/event/CollisionEvent.h"
#include "Enemy.h"
#include "Player.h"
#include "Projectile.h"
#include "lib/util/graphic/Color.h"

uint32_t Projectile::DRAW_LIST_ID = UINT32_MAX;

Projectile::Projectile(const Util::Math::Vector3<float> &position,const Util::Math::Vector3<float> &direction, uint32_t tag) : Entity(tag, position, Util::Math::Vector3<float>(0, 0, 0), Util::Math::Vector3<float>(1, 1, 1), Pulsar::D3::SphereCollider(position, 0.25)), direction(direction), origin(position) {}

Projectile::Projectile(const Util::Math::Vector3<float> &position,const Util::Math::Vector3<float> &direction, uint32_t tag, float range) : Pulsar::D3::Entity(tag, position, Util::Math::Vector3<float>(0, 0, 0), Util::Math::Vector3<float>(1, 1, 1), Pulsar::D3::SphereCollider(position, 0.25)), range(range), direction(direction), origin(position) {}

void Projectile::initialize() {
    if (DRAW_LIST_ID == UINT32_MAX) {
        DRAW_LIST_ID = Pulsar::Graphics::startList3D();
        Pulsar::Graphics::listCuboid3D(Util::Math::Vector3<float>(0.5, 0.5, 0.5));
        Pulsar::Graphics::endList3D();
    }
}

void Projectile::onUpdate(float delta) {
    if (range < Util::Math::absolute(origin.getX() - getPosition().getX()) || range < Util::Math::absolute(origin.getZ() - getPosition().getZ())) {
        removeFromScene();
    }

    setPosition(getPosition() + (direction * delta * 12));
}

void Projectile::draw(Pulsar::Graphics &graphics) const {
    graphics.setColor(getTag() == TAG_PLAYER ? Util::Graphic::Color(68, 195, 212) : Util::Graphic::Color(255, 0, 0));
    graphics.drawList3D(getPosition(), getScale(), getRotation(), DRAW_LIST_ID);
}

void Projectile::onCollisionEvent(const Pulsar::D3::CollisionEvent &event) {
    const auto otherTag = event.getCollidedWidth().getTag();
    if ((getTag() == TAG_PLAYER && otherTag == Enemy::TAG) || (getTag() == TAG_ENEMY && otherTag == Player::TAG)) {
        removeFromScene();
    }
}
