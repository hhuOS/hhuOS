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

#include "Light.h"

namespace Pulsar::D3 {

Light::Light(const uint8_t index, const Type type, const Util::Math::Vector3<float> &position,
    const Util::Graphic::Color &diffuseColor, const Util::Graphic::Color &specularColor) :
    index(index), type(type), position(position), diffuseColor(diffuseColor), specularColor(specularColor) {}

bool Light::isValid() const {
    return index != INVALID_INDEX;
}

uint8_t Light::getIndex() const {
    return index;
}

Light::Type Light::getType() const {
    return type;
}

const Util::Math::Vector3<float> &Light::getPosition() const {
    return position;
}

void Light::setPosition(const Util::Math::Vector3<float> &position) {
    Light::position = position;
}

const Util::Graphic::Color &Light::getDiffuseColor() const {
    return diffuseColor;
}

void Light::setDiffuseColor(const Util::Graphic::Color &diffuseColor) {
    Light::diffuseColor = diffuseColor;
}

const Util::Graphic::Color &Light::getSpecularColor() const {
    return specularColor;
}

void Light::setSpecularColor(const Util::Graphic::Color &specularColor) {
    Light::specularColor = specularColor;
}

}