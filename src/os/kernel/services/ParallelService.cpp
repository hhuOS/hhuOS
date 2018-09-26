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

#include "ParallelService.h"

Logger &ParallelService::log = Logger::get("PARALLEL");

ParallelService::ParallelService() {
    if(Parallel::checkPort(Parallel::LPT1)) {
        log.info("Detected LPT1");
        lpt1 = new Parallel(Parallel::LPT1);
    }

    if(Parallel::checkPort(Parallel::LPT2)) {
        log.info("Detected LPT2");
        lpt2 = new Parallel(Parallel::LPT2);
    }

    if(Parallel::checkPort(Parallel::LPT3)) {
        log.info("Detected LPT3");
        lpt3 = new Parallel(Parallel::LPT3);
    }
}

Parallel *ParallelService::getParallelPort(Parallel::LptPort port) {
    switch(port) {
        case Parallel::LPT1 :
            return lpt1;
        case Parallel::LPT2 :
            return lpt2;
        case Parallel::LPT3 :
            return lpt3;
        default:
            return nullptr;
    }
}

bool ParallelService::isPortAvailable(Parallel::LptPort port) {
    switch(port) {
        case Parallel::LPT1 :
            return lpt1 != nullptr;
        case Parallel::LPT2 :
            return lpt2 != nullptr;
        case Parallel::LPT3 :
            return lpt3 != nullptr;
        default:
            return false;
    }
}
