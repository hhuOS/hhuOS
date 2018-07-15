/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <kernel/services/ParallelService.h>
#include "LptConfig.h"

LptConfig::LptConfig(Shell &shell) : Command(shell) {

};

void LptConfig::execute(Util::Array<String> &args) {
    uint8_t port = 0;
    String mode;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(args[i] == "-h" || args[i] == "--help") {
            stdout << "LptConfigs the screen." << endl << endl;
            stdout << "Usage: " << args[0] << " [OPTION]..." << endl << endl;
            stdout << "Options:" << endl;
            stdout << "  -h, --help: Show this help-message." << endl;
            stdout << "  -p, --port: Set the port (1-3)." << endl;
            stdout << "  -m, --mode: Set the operating mode (spp/epp)." << endl;
            return;
        } else if(args[i] == "-p" || args[i] == "--port") {
            if(i == args.length() - 1) {
                stderr << args[0] << ": '" << args[i] << "': This option needs an argument!" << endl;
                return;
            }

            port = static_cast<uint8_t>(strtoint((const char *) args[++i]));
        } else if(args[i] == "-m" || args[i] == "--mode") {
            if(i == args.length() - 1) {
                stderr << args[0] << ": '" << args[i] << "': This option needs an argument!" << endl;
                return;
            }

            mode = args[++i];
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    if(port < 1 || port > 3) {
        stderr << args[0] << ": Please enter valid port (1-3) using '--port'!" << endl;
        return;
    }

    if(!Kernel::getService<ParallelService>()->isPortAvailable(static_cast<Parallel::LptPort>(port))) {
        stderr << args[0] << ": LPT" << static_cast<uint32_t>(port) << " is not available on this machine!" << endl;
        return;
    }

    Parallel *parallel = Kernel::getService<ParallelService>()->getParallelPort(static_cast<Parallel::LptPort>(port));

    if(!mode.isEmpty()) {
        if (mode == "spp") {
            parallel->setMode(Parallel::ParallelMode::SPP);
        } else if (mode == "epp") {
            parallel->setMode(Parallel::ParallelMode::EPP);
        } else {
            stderr << args[0] << ": Unknown mode '" << mode << "'!" << endl;
            return;
        }
    }

    stdout << "LPT" << static_cast<uint32_t>(port) << ":" << endl;
    stdout << "  Mode: " << (parallel->getMode() == Parallel::ParallelMode::SPP ? "SPP" : "EPP") << endl;
}