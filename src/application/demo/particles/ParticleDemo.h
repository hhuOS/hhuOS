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
 * The particle demo is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_APPLICATION_DEMO_PARTICLEDEMO_H
#define HHUOS_APPLICATION_DEMO_PARTICLEDEMO_H

#include "RainEmitter.h"

#include <pulsar/2d/Scene.h>

/// A demo showcasing the 2D particle system of the Pulsar game engine.
/// A cloud is moving from left to right and vice versa on the upper half of the screen and emits raindrops.
/// When a raindrop hits the ground, or one of the dinosaur sprites standing on the ground,
/// it splashes into multiple smaller droplets.
class ParticleDemo : public Pulsar::D2::Scene {

public:
    /// Create a new particle demo scene instance.
    ParticleDemo() = default;

    /// Initialize the particle demo scene by loading all its objects.
    void initialize() override;

    /// Set the scene's background to a light blue color.
    bool initializeBackground(Pulsar::Graphics &graphics) override;

    /// Exit the demo when the Escape key is pressed.
    void keyPressed(const Util::Io::KeyEvent &key) override;

private:

    RainEmitter *cloud = nullptr;
};

#endif
