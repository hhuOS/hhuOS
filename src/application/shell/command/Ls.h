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

#ifndef __Ls_include__
#define __Ls_include__

#include "lib/stream/OutputStream.h"
#include "lib/string/String.h"
#include "Command.h"

/**
 * Implementation of Command.
 * Takes multiple directories as parameters and lists their contents.
 *
 * -h, --help: Show help message
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class Ls : public Command {

public:
    /**
     * Default-constructor.
     */
    Ls() = delete;

    /**
     * Copy-constructor.
     */
    Ls(const Ls &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Ls(Shell &shell);

    /**
     * Destructor.
     */
    ~Ls() override = default;

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