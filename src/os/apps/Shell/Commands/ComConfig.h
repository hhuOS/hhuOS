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

#ifndef __ComConfig_include__
#define __ComConfig_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include "Command.h"

/**
 * Implementation of Command.
 * Configures a COM-port and prints out the current configuration.
 *
 * -h, --help: Show help message
 * -p, --port: Set the port (1-4)
 * -b, --baud: Set the baud-rate
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class ComConfig : public Command {

public:
    /**
     * Default-constructor.
     */
    ComConfig() = delete;

    /**
     * Copy-constructor.
     */
    ComConfig(const ComConfig &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit ComConfig(Shell &shell);

    /**
     * Destructor.
     */
    ~ComConfig() override = default;

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