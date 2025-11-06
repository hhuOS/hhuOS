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

#ifndef HHUOS_BATTLESPACE_H
#define HHUOS_BATTLESPACE_H

#include "util/io/key/Key.h"

static constexpr const char *INTRO_TEXT =
    "___  ____ ___ ___ _    ____ ____ ___  ____ ____ ____ \n"
    "|__] |__|  |   |  |    |___ [__  |__] |__| |    |___ \n"
    "|__] |  |  |   |  |___ |___ ___] |    |  | |___ |___ \n"
    " \n"
    " \n"
    " \n"
    "Turn using ARROW KEYS. Fire using SPACEBAR.\n"
    "Strafe using WASD. Change speed using Q and E.\n"
    " \n"
    "Press SPACE to start or ESC to exit!\n";

static constexpr const char *GAME_OVER_TEXT =
    "____ ____ _  _ ____    ____ _  _ ____ ____ \n"
    "| __ |__| |\\/| |___    |  | |  | |___ |__/ \n"
    "|__] |  | |  | |___    |__|  \\/  |___ |  \\ \n"
    " \n"
    "Score: %u\n"
    " \n"
    "Press SPACE to play again or ESC to exit!\n";

void handleKeyPressOnTextScreen(const Util::Io::Key &key);

#endif