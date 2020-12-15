/*
 * Copyright (C) 2020 Namit Shah, Martand Javia & Harvish Jariwala
 * Ahmedabad University
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

#ifndef __Rmdir_include__
#define __Rmdir_include__

#include "lib/stream/OutputStream.h"
#include "lib/string/String.h"
#include "lib/file/FileStatus.h"
#include "Command.h"

/** 
 * Implementation of Command.
 * Deletes empty directory.
 *
 * -h, --help: Show help message
 *
 * @author Namit Shah, Martand Javia, Harvish Jariwala
 * @date 2020
 */
class Rmdir : public Command {

private:
    Filesystem *fileSystem = nullptr;
    void deleteFile(const String &progName, FileStatus &fStat);
    void deleteDirectory(const String &progName, Directory &dir);

public:
    /**
     * Default-constructor.
     */
    Rmdir() = delete;
    /**
     * Copy-constructor.
     */
    Rmdir(const Rmdir &copy) = delete;
    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Rmdir(Shell &shell);
    /**
     * Destructor.
     */
    ~Rmdir() override = default;
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