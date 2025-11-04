/*
* Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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
 * The OpenGL demo has been created during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#include "DemoModel.h"

#include "lib/util/pulsar/Graphics.h"
#include "lib/util/base/Panic.h"
#include "lib/util/base/String.h"
#include "lib/util/math/Vector3.h"

uint32_t DemoModel::TREE_DRAW_LIST_ID = UINT32_MAX;
uint32_t DemoModel::LANTERN_DRAW_LIST_ID = UINT32_MAX;
uint32_t DemoModel::ICOSPHERE_DRAW_LIST_ID = UINT32_MAX;

DemoModel::DemoModel(Type type, const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &rotation, const Util::Math::Vector3<double> &scale, const Util::Graphic::Color &color) :
        Model(0, Util::String::format("%s.obj", pathForType(type)), type == ICOSPHERE ? Util::String::format("%s.bmp", pathForType(type)) : Util::String(), position, rotation, scale), type(type), color(color) {}

void DemoModel::initialize() {
    Model::initialize();

    switch (type) {
        case TREE:
            if (TREE_DRAW_LIST_ID == UINT32_MAX) {
                TREE_DRAW_LIST_ID = Util::Pulsar::Graphics::startList3D();
                Util::Pulsar::Graphics::listModel3D(*this);
                Util::Pulsar::Graphics::endList3D();
            }
            drawListID = TREE_DRAW_LIST_ID;
            break;
        case LANTERN:
            if (LANTERN_DRAW_LIST_ID == UINT32_MAX) {
                LANTERN_DRAW_LIST_ID = Util::Pulsar::Graphics::startList3D();
                Util::Pulsar::Graphics::listModel3D(*this);
                Util::Pulsar::Graphics::endList3D();
            }
            drawListID = LANTERN_DRAW_LIST_ID;
            break;
        case ICOSPHERE:
            if (ICOSPHERE_DRAW_LIST_ID == UINT32_MAX) {
                ICOSPHERE_DRAW_LIST_ID = Util::Pulsar::Graphics::startList3D();
                Util::Pulsar::Graphics::listModel3D(*this);
                Util::Pulsar::Graphics::endList3D();
            }
            drawListID = ICOSPHERE_DRAW_LIST_ID;
            break;
        default:
            Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "DemoModel: Invalid type!");
    }
}

void DemoModel::draw(Util::Pulsar::Graphics &graphics) const {
    graphics.setColor(color);
    graphics.drawList3D(getPosition(), getScale(), getRotation(), drawListID);
}

void DemoModel::onUpdate([[maybe_unused]] double delta) {
    if (type == ICOSPHERE) {
        rotate(Util::Math::Vector3<double>(0, 0, 1) * delta * 30);
        translate(getFrontVector() * delta * 10);
    }
}

void DemoModel::onCollisionEvent([[maybe_unused]] Util::Pulsar::D3::CollisionEvent &event) {}

const char* DemoModel::pathForType(Type type) {
    switch (type) {
        case TREE:
            return "/user/demo/tree";
        case LANTERN:
            return "/user/demo/lantern";
        case ICOSPHERE:
            return "/user/demo/icosphere";
        default:
            Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "DemoModel: Invalid type!");
    }
}
