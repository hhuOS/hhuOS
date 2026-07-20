/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

    static int64_t systemCallMount(const char *deviceName, const char *targetPath, const char *driverName);

    static int64_t systemCallUnmount(const char *path);

    static int64_t systemCallOpenFile(const char *path);

    static int64_t systemCallCloseFile(int32_t fileDescriptor);

    static int64_t systemCallCreateFile(const char *path, Util::Io::File::Type type);

    static int64_t systemCallDeleteFile(const char *path);

    static int64_t systemCallGetFileType(int32_t fileDescriptor);

    static int64_t systemCallGetFileLength(int32_t fileDescriptor);

    static int64_t systemCallGetFileChildren(int32_t fileDescriptor, const char ***targetChildren);

    static int64_t systemCallWriteFile(int32_t fileDescriptor, const uint8_t *sourceBuffer, uint32_t posLow, uint32_t posHigh, uint32_t length);

    static int64_t systemCallReadFile(int32_t fileDescriptor, uint8_t *sourceBuffer, uint32_t posLow, uint32_t posHigh, uint32_t length);

    static int64_t systemCallControlFile(int32_t fileDescriptor, uint32_t request, uint32_t arg0, uint32_t arg1, uint32_t arg2);

    static int64_t systemCallControlFileDescriptor(int32_t fileDescriptor, uint32_t request, uint32_t arg0, uint32_t arg1, uint32_t arg2);

    static int64_t systemCallChangeDirectory(const char *path);

    static int64_t systemCallGetCurrentWorkingDirectory();

    Filesystem::Filesystem filesystem;
};

}

#endif