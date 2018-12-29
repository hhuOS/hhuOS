/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include "FloppyModule.h"

MODULE_PROVIDER {

    return new FloppyModule();
};

int32_t FloppyModule::initialize() {

    log = &Logger::get("FLOPPY");

    if(FloppyController::isAvailable()) {
        log->info("Floppy controller is available and at least one drive is attached to it");

        controller = new FloppyController();
        controller->plugin();
        controller->setup();
    } else {
        log->info("No floppy drives available");
    }

    return 0;
}

int32_t FloppyModule::finalize() {
    return 0;
}

String FloppyModule::getName() {
    return String();
}

Util::Array<String> FloppyModule::getDependencies() {
    return Util::Array<String>(0);
}