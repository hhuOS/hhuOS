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
 */

#ifndef HHUOS_BUG_DEFENDER_INTROSCREEN_H
#define HHUOS_BUG_DEFENDER_INTROSCREEN_H

#include "lib/util/game/2d/Scene.h"
#include "lib/util/game/KeyListener.h"

class IntroScreen : public Util::Game::D2::Scene, public Util::Game::KeyListener {

public:
    /**
     * Default Constructor.
     */
    explicit IntroScreen() = default;

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

    static const constexpr char *INTRO_TEXT[9] = {
            "___  _  _ ____    ___  ____ ____ ____ _  _ ___  ____ ____ ",
            "|__] |  | | __    |  \\ |___ |___ |___ |\\ | |  \\ |___ |__/ ",
            "|__] |__| |__]    |__/ |___ |    |___ | \\| |__/ |___ |  \\ ",
            "",
            "",
            "",
            "Move using ARROW KEYS. Fire using SPACEBAR.",
            "",
            "Press SPACE to start or ESC to exit!"
    };
};

#endif
