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

#ifndef HHUOS_APPLICATION_TINYGL_TINYGLDEMO_H
#define HHUOS_APPLICATION_TINYGL_TINYGLDEMO_H

#include <util/io/key/KeyEvent.h>

/// Base class for TinyGL demos.
/// The main render loop is driven by the application's `main()` function, which calls `TinyGlDemo::initialize()`
/// at the beginning, and `TinyGlDemo::renderFrame()` during each iteration.
class TinyGlDemo {

public:
    /// Create a new TinyGL demo instance.
    TinyGlDemo() = default;

    /// Since the base class has no state, the default destructor is sufficient.
    virtual ~TinyGlDemo() = default;

    /// Initialize the demo.
    /// This should perform all OpenGL initialization required to run the demo
    /// (e.g., set up view port, load textures, etc.).
    /// The screen resolution is given as parameters for this method.
    virtual void initialize(uint16_t resX, uint16_t resY) = 0;

    /// Update the scene's state (e.g., object positions, rotation angles, etc.).
    /// This method is called in each iteration of the main render loop before `renderFrame()`.
    /// The given parameter represents the time in seconds since the last call to `update()`.
    virtual void update(float delta) = 0;

    /// Render the demo scene.
    virtual void renderFrame() const = 0;

    /// Handle a key event.
    /// All key events except the Escape key are forwarded to the demo.
    virtual void handleKeyEvent(const Util::Io::KeyEvent&) {}
};

#endif
