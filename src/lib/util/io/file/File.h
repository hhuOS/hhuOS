/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_BMP_FILE_H
#define HHUOS_BMP_FILE_H

#include <cstdint>

#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"

namespace Util::Io {

class File {

public:
    /**
     * Enumeration of different file types.
     */
    enum Type : uint8_t {
        REGULAR,
        DIRECTORY,
        CHARACTER
    };

    /**
     * Constructor.
     */
    explicit File(const String &path);

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

    [[nodiscard]] String getName() const;

    [[nodiscard]] String getCanonicalPath() const;

    [[nodiscard]] String getParent() const;

    [[nodiscard]] Array<String> getChildren();

    [[nodiscard]] File getParentFile() const;

    [[nodiscard]] bool create(Type fileType);

    [[nodiscard]] bool remove();

    [[nodiscard]] bool control(uint32_t request, const Util::Array<uint32_t> &parameters);

    [[nodiscard]] static String getCanonicalPath(const Util::String &path);

    [[nodiscard]] static File getCurrentWorkingDirectory();

    [[nodiscard]] static const char* getTypeColor(Util::Io::File &file);

    int32_t static open(const Util::String &path);

    bool static control(int32_t fileDescriptor, uint32_t request, const Util::Array<uint32_t> &parameters);

    void static close(int32_t fileDescriptor);

    static bool mount(const Util::String &device, const Util::String &targetPath, const Util::String &driverName);

    static bool unmount(const Util::String &path);

    static bool changeDirectory(const Util::String &path);

    static constexpr const char *SEPARATOR = "/";

private:

    String path;
    int32_t fileDescriptor = -1;

    void ensureFileIsOpened();
};

static const constexpr int32_t STANDARD_OUTPUT = 0;
static const constexpr int32_t STANDARD_INPUT = 1;
static const constexpr int32_t STANDARD_ERROR = 2;

}

#endif
