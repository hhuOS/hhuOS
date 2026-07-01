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

#ifndef HHUOS_APPLICATION_OPENGLDEMO_H
#define HHUOS_APPLICATION_OPENGLDEMO_H

#include <util/math/Vector3.h>
#include <pulsar/3d/Scene.h>

/// A 3D demo scene rendered by the Pulsar game engine using TinyGL.
/// It renders some colored and textured objects and allows the user to move around freely
/// using the arrow keys and WASD.
class OpenGlDemo : public Pulsar::D3::Scene {

public:
    /// Create a new demo scene instance.
    /// No entities are created yet. This is done by `initialize()`.
    OpenGlDemo() = default;

    /// Initialize the demo scene by loading all its objects.
    void initialize() override;

    /// Update the scene by translating and rotating the camera according to the user input.
    void update(float delta) override;

    /// Handle key presses (e.g., by modifying the translation/rotation values applied to the camera during `update()`).
    void keyPressed(const Util::Io::KeyEvent &key) override;

    /// Handle key presses (e.g., by modifying the translation/rotation values applied to the camera during `update()`).
    void keyReleased(const Util::Io::KeyEvent &key) override;

private:

    Util::Math::Vector3<float> cameraRotation;
    Util::Math::Vector3<float> cameraTranslation;
};

#endif
