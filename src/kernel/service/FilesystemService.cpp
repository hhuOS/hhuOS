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

#include <stdarg.h>

#include "kernel/system/System.h"
#include "ProcessService.h"
#include "FilesystemService.h"
#include "filesystem/core/Node.h"
#include "kernel/file/FileDescriptorManager.h"
#include "kernel/process/Process.h"
#include "kernel/service/MemoryService.h"
#include "kernel/system/SystemCall.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/System.h"

namespace Kernel {

FilesystemService::FilesystemService() {
    SystemCall::registerSystemCall(Util::System::MOUNT, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 3) {
            return false;
        }

        auto &filesystemService = System::getService<FilesystemService>();
        auto *deviceName = va_arg(arguments, const char*);
        auto *targetPath = va_arg(arguments, const char*);
        auto *driverName = va_arg(arguments, const char*);

        return filesystemService.mount(deviceName, targetPath, driverName);
    });

    SystemCall::registerSystemCall(Util::System::UNMOUNT, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &filesystemService = System::getService<FilesystemService>();
        auto *path = va_arg(arguments, const char*);

        return filesystemService.unmount(path);
    });

    SystemCall::registerSystemCall(Util::System::OPEN_FILE, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 2) {
            return false;
        }

        auto &filesystemService = System::getService<FilesystemService>();
        auto *path = va_arg(arguments, const char*);
        auto &fileDescriptor = *va_arg(arguments, int32_t*);

        fileDescriptor = filesystemService.openFile(path);
        return true;
    });

    SystemCall::registerSystemCall(Util::System::CLOSE_FILE, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &filesystemService = System::getService<FilesystemService>();
        auto fileDescriptor = va_arg(arguments, int32_t);

        filesystemService.closeFile(fileDescriptor);
        return true;
    });

    SystemCall::registerSystemCall(Util::System::CREATE_FILE, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 2) {
            return false;
        }

        auto *path = va_arg(arguments, const char*);
        auto type = static_cast<Util::Io::File::Type>(va_arg(arguments, uint32_t));

        if (type != Util::Io::File::REGULAR && type != Util::Io::File::DIRECTORY) {
            return false;
        }

        auto &filesystemService = System::getService<FilesystemService>();
        return type == Util::Io::File::REGULAR ? filesystemService.createFile(path) : filesystemService.createDirectory(path);
    });

    SystemCall::registerSystemCall(Util::System::DELETE_FILE, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &filesystemService = System::getService<FilesystemService>();
        auto *path = va_arg(arguments, const char*);

        return filesystemService.deleteFile(path);
    });

    SystemCall::registerSystemCall(Util::System::FILE_TYPE, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 2) {
            return false;
        }

        auto fileDescriptor = va_arg(arguments, int32_t);
        auto &type = *va_arg(arguments, Util::Io::File::Type*);

        type = System::getService<FilesystemService>().getNode(fileDescriptor).getType();
        return true;
    });

    SystemCall::registerSystemCall(Util::System::FILE_LENGTH, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 2) {
            return false;
        }

        auto &filesystemService = System::getService<FilesystemService>();
        auto fileDescriptor = va_arg(arguments, int32_t);
        auto &length = *va_arg(arguments, uint64_t*);

        length = filesystemService.getNode(fileDescriptor).getLength();
        return true;
    });

    SystemCall::registerSystemCall(Util::System::FILE_CHILDREN, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 3) {
            return false;
        }

        auto &filesystemService = System::getService<FilesystemService>();
        auto &memoryService = System::getService<MemoryService>();
        auto fileDescriptor = va_arg(arguments, int32_t);
        auto **&targetChildren = *va_arg(arguments, char***);
        auto &count = *va_arg(arguments, uint32_t*);

        auto children = filesystemService.getNode(fileDescriptor).getChildren();
        count = children.length();
        targetChildren = static_cast<char**>(memoryService.allocateUserMemory(children.length() * sizeof(char*)));

        for (uint32_t i = 0; i < children.length(); i++) {
            targetChildren[i] = static_cast<char*>(memoryService.allocateUserMemory((children[i].length() + 1) * sizeof(char)));
            auto source = Util::Address<uint32_t>(static_cast<const char*>(children[i]));
            auto target = Util::Address<uint32_t>(targetChildren[i]);
            target.copyString(source);
        }

        return true;
    });

    SystemCall::registerSystemCall(Util::System::WRITE_FILE, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 5) {
            return false;
        }

        auto &filesystemService = System::getService<FilesystemService>();
        auto fileDescriptor = va_arg(arguments, int32_t);
        auto *sourceBuffer = va_arg(arguments, uint8_t*);
        auto pos = va_arg(arguments, uint64_t);
        auto length = va_arg(arguments, uint64_t);
        auto &written = *va_arg(arguments, uint64_t*);

        written = filesystemService.getNode(fileDescriptor).writeData(sourceBuffer, pos, length);
        return true;
    });

    SystemCall::registerSystemCall(Util::System::READ_FILE, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 5) {
            return false;
        }

        auto &filesystemService = System::getService<FilesystemService>();
        auto fileDescriptor = va_arg(arguments, int32_t);
        auto *targetBuffer = va_arg(arguments, uint8_t*);
        auto pos = va_arg(arguments, uint64_t);
        auto length = va_arg(arguments, uint64_t);
        auto &read = *va_arg(arguments, uint64_t*);

        read = filesystemService.getNode(fileDescriptor).readData(targetBuffer, pos, length);
        return true;
    });

    SystemCall::registerSystemCall(Util::System::CONTROL_FILE, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 3) {
            return false;
        }

        auto &filesystemService = System::getService<FilesystemService>();
        auto fileDescriptor = va_arg(arguments, int32_t);
        auto request = va_arg(arguments, uint32_t);
        auto &parameters = *va_arg(arguments, const Util::Array<uint32_t>*);

        return filesystemService.getNode(fileDescriptor).control(request, parameters);
    });

    SystemCall::registerSystemCall(Util::System::CHANGE_DIRECTORY, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &processService = System::getService<ProcessService>();
        auto *path = va_arg(arguments, const char*);

        return processService.getCurrentProcess().setWorkingDirectory(path);
    });

    SystemCall::registerSystemCall(Util::System::GET_CURRENT_WORKING_DIRECTORY, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &processService = System::getService<ProcessService>();
        auto &memoryService = System::getService<MemoryService>();
        auto *&targetPath = *va_arg(arguments, char**);
        auto path = processService.getCurrentProcess().getWorkingDirectory().getCanonicalPath();

        targetPath = static_cast<char*>(memoryService.allocateUserMemory((path.length() + 1) * sizeof(char)));
        auto source = Util::Address<uint32_t>(static_cast<char*>(path));
        auto target = Util::Address<uint32_t>(targetPath);
        target.copyString(source);

        return true;
    });
}

bool FilesystemService::mount(const Util::String &deviceName, const Util::String &targetPath, const Util::String &driverName) {
    return filesystem.mount(deviceName, targetPath, driverName);
}

bool FilesystemService::unmount(const Util::String &path) {
    return filesystem.unmount(path);
}

bool FilesystemService::createFilesystem(const Util::String &deviceName, const Util::String &driverName) {
    return filesystem.createFilesystem(deviceName, driverName);
}

bool FilesystemService::createFile(const Util::String &path) {
    return filesystem.createFile(path);
}

bool FilesystemService::createDirectory(const Util::String &path) {
    return filesystem.createDirectory(path);
}

bool FilesystemService::deleteFile(const Util::String &path) {
    return filesystem.deleteFile(path);
}

int32_t FilesystemService::openFile(const Util::String &path) {
    return System::getService<ProcessService>().getCurrentProcess().getFileDescriptorManager().openFile(path);
}

int32_t FilesystemService::registerFile(Filesystem::Node *node) {
    return System::getService<ProcessService>().getCurrentProcess().getFileDescriptorManager().registerFile(node);
}

void FilesystemService::closeFile(int32_t fileDescriptor) {
    return System::getService<ProcessService>().getCurrentProcess().getFileDescriptorManager().closeFile(fileDescriptor);
}

Filesystem::Node& FilesystemService::getNode(int32_t fileDescriptor) {
    return System::getService<ProcessService>().getCurrentProcess().getFileDescriptorManager().getNode(fileDescriptor);
}

Filesystem::Filesystem& FilesystemService::getFilesystem() {
    return filesystem;
}

Util::Array<Filesystem::MountInformation> FilesystemService::getMountInformation() {
    return filesystem.getMountInformation();
}

}