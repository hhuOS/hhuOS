/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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
 */

#ifndef HHUOS_BATTLESPACEINTRO_H
#define HHUOS_BATTLESPACEINTRO_H

#include "lib/util/game/2d/Scene.h"
#include "lib/util/game/KeyListener.h"

class IntroScreen : public Util::Game::D2::Scene, public Util::Game::KeyListener {

public:
    /**
     * Default Constructor.
     */
    IntroScreen();

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

    void initializeBackground(Util::Game::Graphics &graphics) override;

    void update(double delta) override;

    void keyPressed(Util::Io::Key key) override;

    void keyReleased(Util::Io::Key key) override;

private:

    static const constexpr char *INTRO_TEXT[10] = {
            "___  ____ ___ ___ _    ____ ____ ___  ____ ____ ____ ",
            "|__] |__|  |   |  |    |___ [__  |__] |__| |    |___ ",
            "|__] |  |  |   |  |___ |___ ___] |    |  | |___ |___ ",
            "",
            "",
            "",
            "Turn using ARROW KEYS or the MOUSE. Fire using SPACEBAR.",
            "Strafe using WASD. Change speed using Q and E.",
            "",
            "Press any key to start, ESC to exit!"
    };
};

#endif
