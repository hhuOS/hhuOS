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
 */

#ifndef HHUOS_APPLICATION_DEMO_RECTANGLE_H
#define HHUOS_APPLICATION_DEMO_RECTANGLE_H

#include <util/base/String.h>
#include <util/graphic/Color.h>
#include <util/math/Vector3.h>
#include <pulsar/3d/Entity.h>
#include <pulsar/3d/Texture.h>

/// A 2D rectangle placed in a 3D world that rotates around its center.
/// It can either be filled with a color or a texture.
class Rectangle : public Pulsar::D3::Entity {

public:
    /// Create a new rectangle instance with the given position, size, and initial rotation.
    /// The rotation angle parameter is applied on each update cycle (multiplied with the time delta).
    /// The rectangle is filled with the given color.
    Rectangle(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &startRotation,
        const Util::Math::Vector3<float> &rotationAngle, const Util::Math::Vector2<float> &size,
        const Util::Graphic::Color &color);

    /// Create a new rectangle instance with the given position, size, and initial rotation.
    /// The rotation angle parameter is applied on each update cycle (multiplied with the time delta).
    /// The rectangle is filled with the given texture.
    Rectangle(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &startRotation,
        const Util::Math::Vector3<float> &rotationAngle, const Util::Math::Vector2<float> &size,
        const Util::String &texturePath);

    /// Initialize the rectangle instance, loading its texture (if it has one).
    void initialize() override;

    /// Rotate the rectangle according to its rotation angle and the given delta time.
    void onUpdate(float delta) override;

    /// Draw the rectangle.
    void draw(Pulsar::Graphics &graphics) const override;

private:

    const Util::Math::Vector3<float> rotationAngle;

    const Util::Graphic::Color color;

    const Util::String texturePath;
    Pulsar::D3::Texture texture;
};

#endif
