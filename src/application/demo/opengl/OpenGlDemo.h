/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef OPENGLDEMO_H
#define OPENGLDEMO_H

#include <lib/util/game/KeyListener.h>

#include "lib/util/game/3d/Scene.h"
#include "lib/util/math/Vector3.h"

class OpenGlDemo : public Util::Game::D3::Scene, public Util::Game::KeyListener {

public:
    /**
     * Default Constructor.
     */
    OpenGlDemo() = default;

    /**
     * Copy Constructor.
     */
    OpenGlDemo(const OpenGlDemo &other) = delete;

    /**
     * Assignment operator.
     */
    OpenGlDemo &operator=(const OpenGlDemo &other) = delete;

    /**
     * Destructor.
     */
    ~OpenGlDemo() override = default;

    void initialize() override;

    void update(double delta) override;

    void keyPressed(const Util::Io::Key &key) override;

    void keyReleased(const Util::Io::Key &key) override;

private:

    Util::Math::Vector3<double> cameraRotation = Util::Math::Vector3<double>(0, 0, 0);
    Util::Math::Vector3<double> cameraTranslation = Util::Math::Vector3<double>(0, 0, 0);
};

#endif
