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

#ifndef __MkPart_include__
#define __MkPart_include__

#include "lib/stream/OutputStream.h"
#include "lib/string/String.h"
#include "Command.h"

/**
 * Implementation of Command.
 * Adds a new partition to a device, or overwrites an existing one.
 *
 * -n, --number: The partition number
 * -i, --id: The system id
 * -b, --bootable: Whether or not the partition should be bootable (true/false)
 * -s, --start: The start sector
 * -e, --end: The end sector
 * -h, --help: Show help message
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class MkPart : public Command {

private:
    Kernel::StorageService *storageService = nullptr;
    Filesystem *fileSystem = nullptr;

public:
    /**
     * Default-constructor.
     */
    MkPart() = delete;

    /**
     * Copy-constructor.
     */
    MkPart(const MkPart &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit MkPart(Shell &shell);

    /**
     * Destructor.
     */
    ~MkPart() override = default;

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
