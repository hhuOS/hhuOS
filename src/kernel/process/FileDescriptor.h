/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_FILEDESCRIPTOR_H
#define HHUOS_FILEDESCRIPTOR_H

#include <stdint.h>

#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"

namespace Filesystem {
class Node;
}  // namespace Filesystem

namespace Kernel {

class FileDescriptor {

public:
    /**
     * Default Constructor.
     */
    FileDescriptor() = default;

    /**
     * Constructor.
     */
    FileDescriptor(Filesystem::Node *node, Util::Io::File::AccessMode accessMode);

    /**
     * Copy Constructor.
     */
    FileDescriptor(const FileDescriptor &other) = delete;

    /**
     * Assignment operator.
     */
    FileDescriptor &operator=(const FileDescriptor &other) = delete;

    /**
     * Destructor.
     */
    ~FileDescriptor();

    bool control(uint32_t request, const Util::Array<uint32_t> &parameters);

    [[nodiscard]] bool isValid() const;

    [[nodiscard]] Filesystem::Node& getNode() const;

    [[nodiscard]] Util::Io::File::AccessMode getAccessMode() const;

    void setNode(Filesystem::Node *node);

    void setAccessMode(Util::Io::File::AccessMode accessMode);

    void clear();

private:

    Filesystem::Node *node = nullptr;
    Util::Io::File::AccessMode accessMode = Util::Io::File::BLOCKING;
};

}

#endif
