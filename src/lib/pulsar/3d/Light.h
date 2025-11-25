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

#ifndef HHUOS_LIB_PULSAR_3D_LIGHT_H
#define HHUOS_LIB_PULSAR_3D_LIGHT_H

#include <stdint.h>

#include "util/graphic/Colors.h"
#include "util/math/Vector3.h"
#include "util/graphic/Color.h"

namespace Pulsar::D3 {

/// An OpenGL light source in 3D space.
/// TinyGL supports a maximum of 16 light sources (GL_LIGHT0 to GL_LIGHT15).
/// This class stores an index to identify the light source, its type (directional or point light),
/// its position in 3D space, and its diffuse and specular colors.
/// It is not intended to be created directly by the user, but rather by the `D3::Scene` class,
/// which returns a reference to the newly created light, when `D3::Scene::addLight()` is called.
/// This light can then be modified using the provided setter methods.
class Light {

public:
    /// The type of the light source.
    enum Type {
        /// A directional light source, which has a direction but no position.
        DIRECTIONAL = 0,
        /// A point light source, which has a position in 3D space.
        POINT = 1
    };

    /// Create a new invalid light instance.
    /// This constructor is needed to create an array of lights in the `D3::Scene` class.
    Light() = default;

    /// Create a new light instance with the given parameters.
    /// The index must be between 0 and 15 (inclusive) and is determined by the `D3::Scene` class,
    /// depending on how many lights are already present in the scene.
    Light(uint8_t index, Type type, const Util::Math::Vector3<float> &position,
        const Util::Graphic::Color &diffuseColor, const Util::Graphic::Color &specularColor);

    /// Check if the light is valid (i.e., has a valid index).
    [[nodiscard]] bool isValid() const;

    /// Get the index of the light source (0-15).
    /// This index corresponds to the OpenGL light enums (GL_LIGHT0 to GL_LIGHT15)
    /// and is used by the `Graphics` class to set up the light in OpenGL.
    [[nodiscard]] uint8_t getIndex() const;

    /// Get the type of the light source (directional or point light).
    [[nodiscard]] Type getType() const;

    /// Get the position of the light source in 3D space, or its direction if it is a directional light.
    [[nodiscard]] const Util::Math::Vector3<float>& getPosition() const;

    /// Set the position of the light source in 3D space, or its direction if it is a directional light.
    void setPosition(const Util::Math::Vector3<float> &position);

    /// Get the diffuse color of the light source.
    [[nodiscard]] const Util::Graphic::Color& getDiffuseColor() const;

    /// Set the diffuse color of the light source.
    void setDiffuseColor(const Util::Graphic::Color &diffuseColor);

    /// Get the specular color of the light source.
    [[nodiscard]] const Util::Graphic::Color& getSpecularColor() const;

    /// Set the specular color of the light source.
    void setSpecularColor(const Util::Graphic::Color &specularColor);

private:

    static constexpr uint8_t INVALID_INDEX = 0xff;

    uint8_t index = INVALID_INDEX;
    Type type = POINT;

    Util::Math::Vector3<float> position;
    Util::Graphic::Color diffuseColor = Util::Graphic::Colors::WHITE;
    Util::Graphic::Color specularColor = Util::Graphic::Colors::WHITE;
};

}

#endif
