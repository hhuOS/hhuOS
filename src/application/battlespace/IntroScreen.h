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
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
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

    void initializeBackground(Util::Game::Graphics &graphics) override;

    void update(double delta) override;

    void keyPressed(const Util::Io::Key &key) override;

    void keyReleased(const Util::Io::Key &key) override;

private:

    static const constexpr char *INTRO_TEXT[10] = {
            "___  ____ ___ ___ _    ____ ____ ___  ____ ____ ____ ",
            "|__] |__|  |   |  |    |___ [__  |__] |__| |    |___ ",
            "|__] |  |  |   |  |___ |___ ___] |    |  | |___ |___ ",
            "",
            "",
            "",
            "Turn using ARROW KEYS. Fire using SPACEBAR.",
            "Strafe using WASD. Change speed using Q and E.",
            "",
            "Press SPACE to start or ESC to exit!"
    };
};

#endif
