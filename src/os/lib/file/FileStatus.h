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

#ifndef __FileStatus_include__
#define __FileStatus_include__

#include <lib/String.h>
#include <filesystem/FsNode.h>
#include <filesystem/FileSystem.h>
#include <kernel/Kernel.h>

/**
 * Check if a file or directory exists and get Meta-information about it.
 *
 * Call FileStatus::stat() to open a file.
 * To close a file, just delete the pointer.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class FileStatus {

private:
    Util::SmartPointer<FsNode> node;
    String path;

    /**
     * Constructor.
     *
     * @param node The node, representing the file
     */
    FileStatus(Util::SmartPointer<FsNode> node, const String &path);

public:
    /**
     * Default-constructor.
     */
    FileStatus() = delete;

    /**
     * Copy-constructor.
     */
    FileStatus(const FileStatus &copy) = delete;

    /**
     * Destructor.
     */
    ~FileStatus() = default;

    /**
     * Check, if a file or directory at a specified path exists.
     *
     * @param path The path
     *
     * @return true, if the file exists
     */
    static bool exists(const String &path);

    /**
     * Open a file.
     * CAUTION: May return nullptr, if the file does not exist.
     *          Always check the return value!
     *
     * @param path The path
     *
     * @return The FileStatus-object (or nulltpr on failure)
     */
    static FileStatus *stat(const String &path);

    /**
     * Get the filename.
     */
    String getName();

    /**
     * Get the absolute path, that points to the file
     */
    String getAbsolutePath();

    /**
     * Get the file type.
     */
    uint32_t getFileType();
};

#endif