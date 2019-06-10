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

#ifndef __Touch_include__
#define __Touch_include__

#include "lib/stream/OutputStream.h"
#include "lib/string/String.h"
#include "Command.h"

/**
 * Implementation of Command.
 * Creates empty files. Multiple paths can be handed over.
 *
 * -h, --help: Show help message
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class Touch : public Command {

private:
    Filesystem *fileSystem = nullptr;

public:
    /**
     * Default-constructor.
     */
    Touch() = delete;

    /**
     * Copy-constructor.
     */
    Touch(const Touch &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Touch(Shell &shell);

    /**
     * Destructor.
     */
    ~Touch() override = default;

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