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

#ifndef HHUOS_LIB_PULSAR_CAMERA_H
#define HHUOS_LIB_PULSAR_CAMERA_H

#include "pulsar/3d/Orientation.h"
#include "util/math/Vector3.h"

namespace Pulsar {

/// The camera determines the viewport of the current scene.
/// It has a position and orientation in 3D space, and provides methods to manipulate them.
/// It also provides methods to get the front, up and right vectors based on its orientation.
/// This class works for both 2D and 3D scenes, but in 2D scenes,
/// only the position's X and Y components are relevant (rotation is ignored).
/// Each scene has exactly one camera, which is automatically created when the scene is created
/// and can be accessed via `Scene::getCamera()`.
class Camera {

public:
    /// Create a new camera instance at the origin (0, 0), with no rotation.
    Camera() {
        reset();
    }

    /// Get the current position of the camera.
    const Util::Math::Vector3<float>& getPosition() const {
        return position;
    }

    /// Get the current rotation of the camera.
    const Util::Math::Vector3<float>& getRotation() const {
        return orientation.getRotation();
    }

    /// Set the rotation of the camera in 3D space.
    void setRotation(const Util::Math::Vector3<float> &angle) {
        orientation.setRotation(angle);
    }

    /// Rotate the camera by the given angles in 3D space.
    void rotate(const Util::Math::Vector3<float> &angle) {
        orientation.rotate(angle);
    }

    /// Get the front vector of the camera, based on its orientation.
    /// This vector points in the direction the camera is facing in 3D space.
    const Util::Math::Vector3<float>& getFrontVector() const {
        return orientation.getFront();
    }

    /// Get the up vector of the camera, based on its orientation.
    /// This vector points upwards from the camera's perspective in 3D space.
    const Util::Math::Vector3<float>& getUpVector() const {
        return orientation.getUp();
    }

    /// Get the right vector of the camera, based on its orientation.
    /// This vector points to the right from the camera's perspective in 3D space.
    const Util::Math::Vector3<float>& getRightVector() const {
        return orientation.getRight();
    }

    /// Set the position of the camera in 3D space.
    void setPosition(const Util::Math::Vector3<float> &position) {
        Camera::position = position;
    }

    /// Set the position of the camera in 2D space (Z component is set to 0).
    void setPosition(const Util::Math::Vector2<float> &position) {
        Camera::position = Util::Math::Vector3<float>(position.getX(), position.getY(), 0);
    }

    /// Translate the camera by the given vector in 3D space.
    void translate(const Util::Math::Vector3<float> &translation) {
        position = position + translation;
    }

    /// Translate the camera by the given vector in 2D space.
    void translate(const Util::Math::Vector2<float> &translation) {
        position = position + Util::Math::Vector3<float>(translation.getX(), translation.getY(), 0);
    }

    /// Reset the camera to the origin (0, 0, 0) with no rotation.
    void reset() {
        position = Util::Math::Vector3<float>();
        orientation.reset();
    }

private:

    Util::Math::Vector3<float> position;
    D3::Orientation orientation;
};

}

#endif
