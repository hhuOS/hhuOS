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

#ifndef HHUOS_BUG_DEFENDER_INTROSCREEN_H
#define HHUOS_BUG_DEFENDER_INTROSCREEN_H

#include "lib/pulsar/2d/Scene.h"

class IntroScreen : public Pulsar::D2::Scene {

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

    void initializeBackground(Pulsar::Graphics &graphics) override;

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
