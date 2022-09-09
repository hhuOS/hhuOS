/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef HHUOS_FILE_H
#define HHUOS_FILE_H

#include "lib/util/memory/String.h"
#include "filesystem/core/Node.h"
#include "lib/util/graphic/Ansi.h"

namespace Util::File {

class File {

public:
    /**
     * Constructor.
     */
    explicit File(const Memory::String &path);

    /**
     * Copy Constructor.
     */
    File(const File &copy);

    /**
     * Assignment operator.
     */
    File& operator=(const File &other);

    /**
     * Destructor.
     */
    ~File();

    [[nodiscard]] bool exists();

    [[nodiscard]] Type getType();

    [[nodiscard]] bool isFile();

    [[nodiscard]] bool isDirectory();

    [[nodiscard]] uint32_t getLength();

    [[nodiscard]] Memory::String getName() const;

    [[nodiscard]] Memory::String getCanonicalPath() const;

    [[nodiscard]] Memory::String getParent() const;

    [[nodiscard]] Data::Array<Memory::String> getChildren();

    [[nodiscard]] File getParentFile() const;

    [[nodiscard]] bool create(Type fileType);

    [[nodiscard]] bool remove();

    [[nodiscard]] static Memory::String getCanonicalPath(const Util::Memory::String &path);

    static constexpr const char *SEPARATOR = "/";

private:

    Memory::String path;
    int32_t fileDescriptor = -1;

    void ensureFileIsOpened();
};

bool mount(const Util::Memory::String &device, const Util::Memory::String &targetPath, const Util::Memory::String &driverName);

bool unmount(const Util::Memory::String &path);

int32_t open(const Util::Memory::String &path);

void close(int32_t fileDescriptor);

bool changeDirectory(const Util::Memory::String &path);

File getCurrentWorkingDirectory();

const char* getFileColor(Util::File::File &path);

}

#endif
