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

#ifndef __Rm_include__
#define __Rm_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include <lib/file/FileStatus.h>
#include "Command.h"

/**
 * Implementation of Command.
 * Deletes files. Multiple paths can be handed over.
 *
 * -r, --recursive: Recursively delete an entire directory
 * -h, --help: Show help message
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class Rm : public Command {

private:
    FileSystem *fileSystem = nullptr;

    void deleteFile(const String &progName, FileStatus &fStat);

    void recursiveDelete(const String &progName, Directory &dir);

public:
    /**
     * Default-constructor.
     */
    Rm() = delete;

    /**
     * Copy-constructor.
     */
    Rm(const Rm &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Rm(Shell &shell);

    /**
     * Destructor.
     */
    ~Rm() override = default;

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;
};

#endif