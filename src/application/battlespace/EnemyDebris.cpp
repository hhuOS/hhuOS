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
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "EnemyDebris.h"

#include "Enemy.h"
#include "lib/util/base/String.h"
#include "lib/util/pulsar/Scene.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/math/Random.h"

EnemyDebris::EnemyDebris(const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &rotation, double scale, uint8_t modelId) : Util::Pulsar::D3::Model(Enemy::TAG, Util::String::format("/user/battlespace/debris%u.obj", modelId), position, rotation, Util::Math::Vector3<double>(scale, scale, scale), Util::Graphic::Colors::RED), modelId(modelId) {}

void EnemyDebris::initialize() {
    Model::initialize();

    switch (modelId) {
        case 1: {
            auto r = Util::Math::Random().getRandomNumber();
            auto r2 = (r - 0.5) * 2.0;
            translateDirection = Util::Math::Vector3<double>(-0.05 - 0.1 * r, 0.05 * r2, 0.05 + 0.05 * r);
            break;
        }
        case 2: {
            auto r = Util::Math::Random().getRandomNumber();
            auto r2 = (r - 0.5) * 2.0;
            translateDirection = Util::Math::Vector3<double>(0.05 + 0.1 * r, 0.05 * r2, 0.05 + 0.05 * r);
            break;
        }
        case 3: {
            auto r = Util::Math::Random().getRandomNumber();
            auto r2 = (r - 0.5) * 2.0;
            translateDirection = Util::Math::Vector3<double>(-0.05 - 0.1 * r, 0.05 * r2, 0.05 + 0.05 * r);
            break;
        }
        default:
            break;
    }

    auto r = Util::Math::Random().getRandomNumber() * 35;;
    rotationDirection = Util::Math::Vector3<double>(r, r, r);
}

void EnemyDebris::onUpdate(double delta) {
    lifetime += delta;
    if (lifetime > 2) {
        removeFromScene();
    }

    translate(translateDirection);
    rotate(rotationDirection);
}

void EnemyDebris::onCollisionEvent([[maybe_unused]] Util::Pulsar::D3::CollisionEvent &event) {}
