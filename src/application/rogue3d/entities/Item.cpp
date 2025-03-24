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
 * The rogue game has been implemented during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#include "Item.h"

#include "Player.h"
#include "lib/util/game/3d/event/CollisionEvent.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Scene.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/collection/Array.h"
#include "lib/util/game/3d/collider/SphereCollider.h"
#include "lib/util/math/Vector3.h"

Util::Array<Util::Math::Vector3<double>> Item::HEART = {
    Util::Math::Vector3<double>(0.0f, 0.0f, 0.0f),
    Util::Math::Vector3<double>(-0.5f, 1.0f, 0.0f),
    Util::Math::Vector3<double>(0.5f, 1.0f, 0.0f),

    Util::Math::Vector3<double>(0.0f, 1.0f, 0.0f),
    Util::Math::Vector3<double>(-0.5f, 1.0f, 0.0f),
    Util::Math::Vector3<double>(-0.25, 1.33f, 0.0f),

    Util::Math::Vector3<double>(0.0f, 1.0f, 0.0f),
    Util::Math::Vector3<double>(0.50f, 1.0f, 0.0f),
    Util::Math::Vector3<double>(0.25, 1.33f, 0.0f)
};

Util::Array<Util::Math::Vector3<double>> Item::DMG_UP = {
    Util::Math::Vector3<double>(-0.5f, 0.0f, 0.0f),
    Util::Math::Vector3<double>(-0.5f, 1.0f, 0.0f),
    Util::Math::Vector3<double>(0.5f, 1.0f, 0.0f),

    Util::Math::Vector3<double>(0.5f, 0.0f, 0.0f),
    Util::Math::Vector3<double>(0.5f, 1.0f, 0.0f),
    Util::Math::Vector3<double>(-0.5f, 1.0f, 0.0f),

    Util::Math::Vector3<double>(0.5f, 1.0f, 0.0f),
    Util::Math::Vector3<double>(0.0f, 1.5f, 0.0f),
    Util::Math::Vector3<double>(-0.5f, 1.0f, 0.0f)
};

uint32_t Item::HEALTH_LIST_ID = UINT32_MAX;
uint32_t Item::DMG_UP_LIST_ID = UINT32_MAX;
uint32_t Item::NEXT_LVL_LIST_ID = UINT32_MAX;

Item::Item(const Util::Math::Vector3<double> &position, uint32_t tag)  : Entity(tag, position, Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(1, 1, 1), Util::Game::D3::SphereCollider(position, 0.25)) {}

void Item::initialize() {
    if (HEALTH_LIST_ID == UINT32_MAX) {
        HEALTH_LIST_ID = Util::Game::Graphics::startList3D();
        Util::Game::Graphics::listCustomShape3D(HEART);
        Util::Game::Graphics::endList3D();
    }

    if (DMG_UP_LIST_ID == UINT32_MAX) {
        DMG_UP_LIST_ID = Util::Game::Graphics::startList3D();
        Util::Game::Graphics::listCustomShape3D(DMG_UP);
        Util::Game::Graphics::endList3D();
    }

    if (NEXT_LVL_LIST_ID == UINT32_MAX) {
        NEXT_LVL_LIST_ID = Util::Game::Graphics::startList3D();
        Util::Game::Graphics::listCuboid3D(Util::Math::Vector3<double>(3, 1, 2));
        Util::Game::Graphics::listCuboid3D(Util::Math::Vector3<double>(0.5, 1, 0), Util::Math::Vector3<double>(2, 1, 2));
        Util::Game::Graphics::listCuboid3D(Util::Math::Vector3<double>(1, 2, 0), Util::Math::Vector3<double>(1, 1, 2));
        Util::Game::Graphics::endList3D();
    }
}

void Item::onUpdate(double delta) {
    if (getTag() == TAG_NEXT_LVL) {
        return;
    }

    if (getScale().getX() >= 2) {
        setScale(Util::Math::Vector3<double>(1.0,1.0,1.0));
    }

    setScale(getScale() * (1 + delta));
}

void Item::draw(Util::Game::Graphics &graphics) {
    switch (getTag()) {
        case TAG_HEALTH_UP:
            graphics.setColor(Util::Graphic::Color(255, 0, 0));
            graphics.drawList3D(getPosition(), getScale(), getRotation(), HEALTH_LIST_ID);
            break;
        case TAG_DMG_UP:
            graphics.setColor(Util::Graphic::Color(131, 4, 255));
            graphics.drawList3D(getPosition(), getScale(), getRotation(), DMG_UP_LIST_ID);
            break;
        case TAG_NEXT_LVL:
            graphics.setColor(Util::Graphic::Color(102, 102, 102));
            graphics.drawList3D(getPosition(), getScale(), getRotation(), NEXT_LVL_LIST_ID);
            break;
        default:
            break;
    }
}

void Item::onCollisionEvent(Util::Game::D3::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == Player::TAG && getTag() != TAG_NEXT_LVL) {
        Util::Game::GameManager::getCurrentScene().removeObject(this);
    }
}


