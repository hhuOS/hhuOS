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

    ArgumentParser parser(getHelpText(), 1);
    parser.addParameter("port", "p", true);
    parser.addParameter("mode", "m", false);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    port = static_cast<uint8_t>(strtoint((const char *) parser.getNamedArgument("port")));
    mode = parser.getNamedArgument("mode");

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

const String LptConfig::getHelpText() {
    return "LptConfigs the screen.\n\n"
           "Usage: lptconfig [OPTION]...\n\n"
           "Options:\n"
           "  -h, --help: Show this help-message.\n"
           "  -p, --port: Set the port (1-3).\n"
           "  -m, --mode: Set the operating mode (spp/epp).";
}
