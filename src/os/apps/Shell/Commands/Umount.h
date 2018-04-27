/*
 * Copyright (C) 2018 Fabian Ruhland
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

#ifndef __Umount_include__
#define __Umount_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include "Command.h"

/**
 * Implementation of Command.
 * Unmounts a storage device from its mount path. Multiple paths can be handed over.
 *
 * -h, --help: Show help message
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class Umount : public Command {

public:
    /**
     * Default-constructor.
     */
    Umount() = delete;

    /**
     * Copy-constructor.
     */
    Umount(const Umount &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Umount(Shell &shell);

    /**
     * Destructor.
     */
    ~Umount() override = default;

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;
};

#endif