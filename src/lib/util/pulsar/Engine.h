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

#ifndef HHUOS_LIB_UTIL_ENGINE_H
#define HHUOS_LIB_UTIL_ENGINE_H

#include <stddef.h>
#include <stdint.h>

#include "async/Runnable.h"
#include "pulsar/Game.h"
#include "pulsar/Graphics.h"
#include "pulsar/Statistics.h"
#include "graphic/LinearFrameBuffer.h"
#include "io/key/KeyDecoder.h"
#include "io/key/MouseDecoder.h"
#include "io/key/layout/DeLayout.h"


namespace Util::Pulsar {

/// The main game engine class, which runs the game loop.
/// It handles input, updates the game state, and triggers rendering.
class Engine final : public Async::Runnable {

public:
    /// Create a new game engine instance.
    /// This is the first component that has to be created to run a game.
    /// After creating the engine, the game instance can be accessed via Game::getInstance()
    /// and scenes can be pushed to the game.
    explicit Engine(const Graphic::LinearFrameBuffer &lfb, uint8_t targetFrameRate = 60, double scaleFactor = 1.0);

    /// There should only be one engine instance per game, so the copy constructor is deleted.
    Engine(const Engine &other) = delete;

    /// There should only be one engine instance per game, so the assignment operator is deleted.
    Engine &operator=(const Engine &other) = delete;

    /// Run the main game loop.
    void run() override;

private:

    void initializeNextScene();

    void updateStatus();

    void drawStatus();

    void checkKeyboard();

    void checkMouse(Io::FileInputStream &mouseInputStream);

    void checkMouseKey(Io::MouseDecoder::Button button, uint8_t lastButtonState, uint8_t currentButtonState) const;

    Graphics graphics;
    Game game;
    Statistics statistics;

    bool showStatus = false;
    Time::Timestamp statusUpdateTimer;
    Statistics::Gather status = statistics.gather();

    uint8_t mouseValues[4]{};
    size_t mouseValueIndex = 0;
    uint8_t lastMouseButtons = 0;

    Io::KeyDecoder keyDecoder = Io::KeyDecoder(Io::DeLayout());
    ArrayList<Io::Key> pressedKeys;

    const size_t targetFrameRate;

    static constexpr const char *LOADING = "Loading...";
};

}

#endif
