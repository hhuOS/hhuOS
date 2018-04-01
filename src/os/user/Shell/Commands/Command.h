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

#ifndef __Command_include__
#define __Command_include__

#include <cstdint>
#include <user/Shell/Shell.h>

/**
 * Base class for a Command, that can be executed by the Shell.
 * When an implementation deals with file/directories, it should always use calcAbsolutePath(), to get an absolute Path,
 * that is accepted by the FileSystem.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class Command {

protected:
    Shell &shell;
    OutputStream &stdout;
    OutputStream &stderr;

    /**
     * Uses the shell's current working directory and a given relative path to assemble an absolute path.
     * If the relative path is already absolute, it will just be returned.
     *
     * @param relativePath The relative path
     *
     * @return The calculated absolute path
     */
    String calcAbsolutePath(const String &relativePath);

public:
    /**
     * Default-constructor.
     */
    Command() = delete;

    /**
     * Copy-constructor.
     */
    Command(const Command &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Command(Shell &shell);

    /**
     * Destructor.
     */
    virtual ~Command() = default;

    /**
     * Execute the command.
     *
     * @param args Parameters, given by the user
     * @param outputStream The stream to write to
     */
    virtual void execute(Util::Array<String> &args) = 0;
};

#endif