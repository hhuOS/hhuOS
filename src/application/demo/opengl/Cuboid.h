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
 * The OpenGL demo has been created during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#ifndef HHUOS_APPLICATION_DEMO_CUBOID_H
#define HHUOS_APPLICATION_DEMO_CUBOID_H

#include <util/base/String.h>
#include <util/math/Vector3.h>
#include <util/graphic/Color.h>
#include <pulsar/3d/Entity.h>
#include <pulsar/3d/Texture.h>

/// A cuboid-shaped 3D object that rotates around its center.
/// Its faces can either be filled with a color or a texture.
class Cuboid : public Pulsar::D3::Entity {

public:
    /// Create a new cuboid instance with the given position, size, and initial rotation.
    /// The rotation angle parameter is applied on each update cycle (multiplied with the time delta).
    /// All faces are filled with the given color.
    Cuboid(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &startRotation,
        const Util::Math::Vector3<float> &rotationAngle, const Util::Math::Vector3<float> &size,
        const Util::Graphic::Color &color);

    /// Create a new cuboid instance with the given position, size, and initial rotation.
    /// The rotation angle parameter is applied on each update cycle (multiplied with the time delta).
    /// All faces are filled with the given texture.
    Cuboid(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &startRotation,
        const Util::Math::Vector3<float> &rotationAngle, const Util::Math::Vector3<float> &size,
        const Util::String &texturePath);

    /// Initialize the cuboid instance, loading its texture (if it has one).
    void initialize() override;

    /// Rotate the cuboid according to its rotation angle and the given delta time.
    void onUpdate(float delta) override;

    /// Draw the cuboid.
    void draw(Pulsar::Graphics &graphics) const override;

private:

    const Util::Math::Vector3<float> rotationAngle;

    const Util::Graphic::Color color;

    const Util::String texturePath;
    Pulsar::D3::Texture texture;
};

#endif
