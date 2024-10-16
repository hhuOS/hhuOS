/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
 * The dino game is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#ifndef HHUOS_DINO_INTROSCREEN_H
#define HHUOS_DINO_INTROSCREEN_H

#include "lib/util/game/2d/Scene.h"
#include "lib/util/game/KeyListener.h"

namespace Util {
namespace Game {
class Graphics;
}  // namespace Game
namespace Io {
class Key;
}  // namespace Io
}  // namespace Util

class IntroScreen : public Util::Game::D2::Scene, public Util::Game::KeyListener {

public:
    /**
     * Default Constructor.
     */
    IntroScreen() = default;

    /**
     * Copy Constructor.
     */
    IntroScreen(const IntroScreen &other) = delete;

    /**
     * Assignment operator.
     */
    IntroScreen &operator=(const IntroScreen &other) = delete;

    /**
     * Destructor.
     */
    ~IntroScreen() override = default;

    void initialize() override;

    void update(double delta) override;

    void initializeBackground(Util::Game::Graphics &graphics) override;

    void keyPressed(const Util::Io::Key &key) override;

    void keyReleased(const Util::Io::Key &key) override;

private:

    static const constexpr char *INTRO_TEXT[11] = {
            "___  _ _  _ ____",
            "|  \\ | |\\ | |  |",
            "|__/ | | \\| |__|",
            "",
            "",
            "",
            "Let dino hatch by pressing SPACE.",
            "Use ARROW KEYS to move and SPACE to jump.",
            "Collect coins to earns points and fruit to finish a level!",
            "",
            "Press SPACE to start or ESC to exit!"
    };
};

#endif
