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

#ifndef __Shutdown_include__
#define __Shutdown_include__

#include "lib/stream/OutputStream.h"
#include "lib/string/String.h"
#include "Command.h"
#include "device/cpu/IoPort.h"

/**
 * Implementation of Command.
 * Shuts down the qemu emulator
 *
 * -h, --help: Show help message
 *
 * @author Namit Shah, Martand Javia & Harvish Jariwala
 * @date 2020
 */

class Shutdown : public Command {

public:
    /**
     * Default-constructor.
     */
    Shutdown() = delete;

    /**
     * Copy-constructor.
     */
    Shutdown(const Shutdown &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Shutdown(Shell &shell);

    /**
     * Destructor.
     */
    ~Shutdown() override = default;

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
