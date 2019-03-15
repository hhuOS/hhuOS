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

#ifndef __Directory_include__
#define __Directory_include__


#include "filesystem/FileSystem.h"
#include "kernel/Kernel.h"
#include "lib/util/Array.h"
#include <cstdint>
/**
 * Allows interacting with a directory.
 *
 * Call Directory::open() to open a directory.
 * To close a directory, just delete the pointer.
 *
 * @author Fabian Ruhland
 * @date 2017
 */
class Directory {

private:
    Util::SmartPointer<FsNode> node;
    String path;

    /**
     * Constructor.
     *
     * @param node The node, representing the directory
     * @param path The absolute path, that points to the directory
     */
    Directory(Util::SmartPointer<FsNode> node, const String &path);

public:
    /**
     * Default-constructor.
     */
    Directory() = delete;

    /**
     * Copy-constructor.
     */
    Directory(const Directory &copy) = delete;

    /**
     * Destructor.
     */
    ~Directory() = default;

    /**
     * Open a directory.
     * CAUTION: May return nullptr, if the directory does not exist.
     *          Always check the return value!
     *
     * @param path The absolute path, that points to the file
     *
     * @return The directory (or nulltpr on failure)
     */
    static Directory *open(const String &path);

    /**
     * Get the directory's name.
     */
    String getName();

    /**
     * Get the absolute path, that points to the directory.
     */
    String getAbsolutePath();

    /**
     * Get the names of the directory's children.
     */
    Util::Array<String> getChildren();
};

#endif