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

#ifndef HHUOS_FILESYSTEMSERVICE_H
#define HHUOS_FILESYSTEMSERVICE_H

#include <stdint.h>

#include "filesystem/Filesystem.h"
#include "Service.h"
#include "lib/util/collection/Array.h"
#include "lib/util/base/String.h"

namespace Filesystem {
class Node;
}  // namespace Filesystem

namespace Kernel {
class FileDescriptor;

class FilesystemService : public Service {

public:
    /**
     * Constructor.
     */
    FilesystemService();

    /**
     * Copy Constructor.
     */
    FilesystemService(const FilesystemService &copy) = delete;

    /**
     * Assignment operator.
     */
    FilesystemService& operator=(const FilesystemService &other) = delete;

    /**
     * Destructor.
     */
    ~FilesystemService() override = default;

    bool mount(const Util::String &deviceName, const Util::String &targetPath, const Util::String &driverName);

    bool unmount(const Util::String &path);

    bool createFilesystem(const Util::String &deviceName, const Util::String &driverName);

    bool createFile(const Util::String &path);

    bool createDirectory(const Util::String &path);

    bool deleteFile(const Util::String &path);

    int32_t registerFile(Filesystem::Node *node);

    int32_t openFile(const Util::String &path);

    void closeFile(int32_t fileDescriptor);

    FileDescriptor& getFileDescriptor(int32_t fileDescriptor);

    Filesystem::Filesystem& getFilesystem();

    Util::Array<Filesystem::MountInformation> getMountInformation();

    static const constexpr uint8_t SERVICE_ID = 0;

private:

    Filesystem::Filesystem filesystem;
};

}

#endif