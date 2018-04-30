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

#ifndef __Echo_include__
#define __Echo_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include "Command.h"

/**
 * Implementation of Command.
 * Writes its parameters to the given output stream.
 *
 * -h, --help: Show help message
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class Echo : public Command {

public:
    /**
     * Default-constructor.
     */
    Echo() = delete;

    /**
     * Copy-constructor.
     */
    Echo(const Echo &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Echo(Shell &shell);

    /**
     * Destructor.
     */
    ~Echo() override = default;

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;
};

#endif