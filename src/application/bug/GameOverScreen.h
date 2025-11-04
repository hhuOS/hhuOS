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
 */

#ifndef HHUOS_BUG_DEFENDER_GAMEOVERSCREEN_H
#define HHUOS_BUG_DEFENDER_GAMEOVERSCREEN_H

#include "lib/util/pulsar/2d/Scene.h"

class GameOverScreen : public Util::Pulsar::D2::Scene {

public:
    /**
     * Constructor.
     */
    explicit GameOverScreen(bool won);

    /**
     * Copy Constructor.
     */
    GameOverScreen(const GameOverScreen &other) = delete;

    /**
     * Assignment operator.
     */
    GameOverScreen &operator=(const GameOverScreen &other) = delete;

    /**
     * Destructor.
     */
    ~GameOverScreen() override = default;

    void initialize() override;

    void update(double delta) override;

    void initializeBackground(Util::Pulsar::Graphics &graphics) override;

    void keyPressed(const Util::Io::Key &key) override;

    void keyReleased(const Util::Io::Key &key) override;

private:

    bool won;

    const char* LOOSE_TEXT[7] = {
            "____ ____ _  _ ____    ____ _  _ ____ ____ ",
            "| __ |__| |\\/| |___    |  | |  | |___ |__/ ",
            "|__] |  | |  | |___    |__|  \\/  |___ |  \\ ",
            "",
            "Your planet has been invaded by aliens!",
            "",
            "Press SPACE to fight the next invasion, or ESC to run like a coward..."
    };

    const char* WIN_TEXT[7] = {
            "____ ____ _  _ ____ ____ ____ ___ _  _ _    ____ ___ _ ____ _  _ ____ ",
            "|    |  | |\\ | | __ |__/ |__|  |  |  | |    |__|  |  | |  | |\\ | [__  ",
            "|___ |__| | \\| |__] |  \\ |  |  |  |__| |___ |  |  |  | |__| | \\| ___] ",
            "",
            "You have stopped the alien invasion!",
            "",
            "Press SPACE to fight the next invasion, or ESC to run like a coward..."
    };
};

#endif
