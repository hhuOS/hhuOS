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

#ifndef __LptConfig_include__
#define __LptConfig_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include "Command.h"

/**
 * Implementation of Command.
 * Configures an LPT-port and prints out the current configuration.
 *
 * -h, --help: Show help message
 * -p, --port: Set the port (1-3)
 * -m, --mode: Set the operating mode (spp/epp)
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class LptConfig : public Command {

public:
    /**
     * Default-constructor.
     */
    LptConfig() = delete;

    /**
     * Copy-constructor.
     */
    LptConfig(const LptConfig &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit LptConfig(Shell &shell);

    /**
     * Destructor.
     */
    ~LptConfig() override = default;

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;

    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;
};

#endif