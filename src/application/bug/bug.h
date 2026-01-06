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
 */

#ifndef HHUOS_BUG_H
#define HHUOS_BUG_H

#include "util/io/key/Key.h"

static constexpr const char *INTRO_TEXT =
    "___  _  _ ____    ___  ____ ____ ____ _  _ ___  ____ ____ \n"
    "|__] |  | | __    |  \\ |___ |___ |___ |\\ | |  \\ |___ |__/ \n"
    "|__] |__| |__]    |__/ |___ |    |___ | \\| |__/ |___ |  \\ \n"
    " \n"
    " \n"
    " \n"
    "Move using ARROW KEYS. Fire using SPACEBAR.\n"
    " \n"
    "Press SPACE to start or ESC to exit!";

static constexpr const char *LOOSE_TEXT =
    "____ ____ _  _ ____    ____ _  _ ____ ____ \n"
    "| __ |__| |\\/| |___    |  | |  | |___ |__/ \n"
    "|__] |  | |  | |___    |__|  \\/  |___ |  \\ \n"
    " \n"
    " \n"
    " \n"
    "Your planet has been invaded by aliens!\n"
    " \n"
    "Press SPACE to fight the next invasion, or ESC to run like a coward...";

static constexpr const char *WIN_TEXT =
    "____ ____ _  _ ____ ____ ____ ___ _  _ _    ____ ___ _ ____ _  _ ____ \n"
    "|    |  | |\\ | | __ |__/ |__|  |  |  | |    |__|  |  | |  | |\\ | [__  \n"
    "|___ |__| | \\| |__] |  \\ |  |  |  |__| |___ |  |  |  | |__| | \\| ___] \n"
    " \n"
    " \n"
    " \n"
    "You have stopped the alien invasion!\n"
    " \n"
    "Press SPACE to fight the next invasion, or ESC to run like a coward...";

void handleKeyPressOnTextScreen(const Util::Io::Key &key);

#endif