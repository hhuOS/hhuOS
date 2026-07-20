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

#include "ProcessService.h"
#include "FilesystemService.h"
#include "filesystem/Node.h"
#include "kernel/process/FileDescriptorManager.h"
#include "kernel/process/Process.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/System.h"
#include "InterruptService.h"
#include "kernel/service/Service.h"
#include "kernel/process/FileDescriptor.h"

namespace Kernel {

FilesystemService::FilesystemService() {
    ASSIGN_SYSTEM_CALL(Util::System::MOUNT, FilesystemService::systemCallMount);
    ASSIGN_SYSTEM_CALL(Util::System::UNMOUNT, FilesystemService::systemCallUnmount);
    ASSIGN_SYSTEM_CALL(Util::System::OPEN_FILE, FilesystemService::systemCallOpenFile);
    ASSIGN_SYSTEM_CALL(Util::System::CLOSE_FILE, FilesystemService::systemCallCloseFile);
    ASSIGN_SYSTEM_CALL(Util::System::CREATE_FILE, FilesystemService::systemCallCreateFile);
    ASSIGN_SYSTEM_CALL(Util::System::DELETE_FILE, FilesystemService::systemCallDeleteFile);
    ASSIGN_SYSTEM_CALL(Util::System::FILE_TYPE, FilesystemService::systemCallGetFileType);
    ASSIGN_SYSTEM_CALL(Util::System::FILE_LENGTH, FilesystemService::systemCallGetFileLength);
    ASSIGN_SYSTEM_CALL(Util::System::FILE_CHILDREN, FilesystemService::systemCallGetFileChildren);
    ASSIGN_SYSTEM_CALL(Util::System::WRITE_FILE, FilesystemService::systemCallWriteFile);
    ASSIGN_SYSTEM_CALL(Util::System::READ_FILE, FilesystemService::systemCallReadFile);
    ASSIGN_SYSTEM_CALL(Util::System::CONTROL_FILE, FilesystemService::systemCallControlFile);
    ASSIGN_SYSTEM_CALL(Util::System::CONTROL_FILE_DESCRIPTOR, FilesystemService::systemCallControlFileDescriptor);
    ASSIGN_SYSTEM_CALL(Util::System::CHANGE_DIRECTORY, FilesystemService::systemCallChangeDirectory);
    ASSIGN_SYSTEM_CALL(Util::System::GET_CURRENT_WORKING_DIRECTORY, FilesystemService::systemCallGetCurrentWorkingDirectory);
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
    return Service::getService<ProcessService>().getCurrentProcess().getFileDescriptorManager().openFile(path);
}

int32_t FilesystemService::registerFile(Filesystem::Node *node) {
    return Service::getService<ProcessService>().getCurrentProcess().getFileDescriptorManager().registerFile(node);
}

void FilesystemService::closeFile(int32_t fileDescriptor) {
    return Service::getService<ProcessService>().getCurrentProcess().getFileDescriptorManager().closeFile(fileDescriptor);
}

FileDescriptor& FilesystemService::getFileDescriptor(int32_t fileDescriptor) {
    return Service::getService<ProcessService>().getCurrentProcess().getFileDescriptorManager().getDescriptor(fileDescriptor);
}

Filesystem::Filesystem& FilesystemService::getFilesystem() {
    return filesystem;
}

Util::Array<Filesystem::MountInformation> FilesystemService::getMountInformation() {
    return filesystem.getMountInformation();
}

int64_t FilesystemService::systemCallMount(const char *deviceName, const char *targetPath, const char *driverName) {
    auto &filesystemService = getService<FilesystemService>();

    return filesystemService.mount(deviceName, targetPath, driverName) ? 0 : -1;
}

int64_t FilesystemService::systemCallUnmount(const char *path) {
    auto &filesystemService = getService<FilesystemService>();

    return filesystemService.unmount(path) ? 0 : -1;
}

int64_t FilesystemService::systemCallOpenFile(const char *path) {
    auto &filesystemService = getService<FilesystemService>();

    return filesystemService.openFile(path);
}

int64_t FilesystemService::systemCallCloseFile(const int32_t fileDescriptor) {
    auto &filesystemService = getService<FilesystemService>();

    filesystemService.closeFile(fileDescriptor);
    return 0;
}

int64_t FilesystemService::systemCallCreateFile(const char *path, const Util::Io::File::Type type) {
    auto &filesystemService = getService<FilesystemService>();

    if (type == Util::Io::File::REGULAR) {
        return filesystemService.createFile(path) ? 0 : -1;
    }

    if (type == Util::Io::File::DIRECTORY) {
        return filesystemService.createDirectory(path) ? 0 : -1;
    }

    return -1;
}

int64_t FilesystemService::systemCallDeleteFile(const char *path) {
    auto &filesystemService = getService<FilesystemService>();

    return filesystemService.deleteFile(path) ? 0 : -1;
}

int64_t FilesystemService::systemCallGetFileType(const int32_t fileDescriptor) {
    auto &filesystemService = getService<FilesystemService>();

    const auto &descriptor = filesystemService.getFileDescriptor(fileDescriptor);
    if (!descriptor.isValid()) {
        return -1;
    }

    return descriptor.getNode().getType();
}

int64_t FilesystemService::systemCallGetFileLength(const int32_t fileDescriptor) {
    auto &filesystemService = getService<FilesystemService>();

    const auto &descriptor = filesystemService.getFileDescriptor(fileDescriptor);
    if (!descriptor.isValid()) {
        return -1;
    }

    return descriptor.getNode().getLength();
}

int64_t FilesystemService::systemCallGetFileChildren(const int32_t fileDescriptor, const char ***targetChildren) {
    auto &filesystemService = getService<FilesystemService>();
    auto &memoryService = getService<MemoryService>();

    const auto &descriptor = filesystemService.getFileDescriptor(fileDescriptor);
    if (!descriptor.isValid()) {
        return -1;
    }

    const auto children = descriptor.getNode().getChildren();
    const auto count = children.length();
    *targetChildren = static_cast<const char**>(memoryService.allocateUserMemory(children.length() * sizeof(char*)));

    for (size_t i = 0; i < children.length(); i++) {
        const char *targetBuffer = static_cast<const char*>(memoryService.allocateUserMemory((children[i].length() + 1) * sizeof(char)));
        auto source = Util::Address(static_cast<const char*>(children[i]));
        auto target = Util::Address(targetBuffer);
        target.copyString(source);

        (*targetChildren)[i] = targetBuffer;
    }

    return count;
}

int64_t FilesystemService::systemCallWriteFile(const int32_t fileDescriptor, const uint8_t *sourceBuffer, const uint32_t posLow, const uint32_t posHigh, const uint32_t length) {
    auto &filesystemService = getService<FilesystemService>();
    const auto pos = static_cast<uint64_t>(posLow) | (static_cast<uint64_t>(posHigh) << 32);

    const auto &descriptor = filesystemService.getFileDescriptor(fileDescriptor);
    if (!descriptor.isValid()) {
        return -1;
    }

    return descriptor.getNode().writeData(sourceBuffer, pos, length);
}

int64_t FilesystemService::systemCallReadFile(const int32_t fileDescriptor, uint8_t *targetBuffer, const uint32_t posLow, const uint32_t posHigh, const uint32_t length) {
    auto &filesystemService = getService<FilesystemService>();
    const auto pos = static_cast<uint64_t>(posLow) | (static_cast<uint64_t>(posHigh) << 32);

    const auto &descriptor = filesystemService.getFileDescriptor(fileDescriptor);
    if (!descriptor.isValid()) {
        return -1;
    }

    if (descriptor.getAccessMode() == Util::Io::File::BLOCKING || descriptor.getNode().isReadyToRead()) {
        return descriptor.getNode().readData(targetBuffer, pos, length);
    }

    return 0;
}

int64_t FilesystemService::systemCallControlFile(const int32_t fileDescriptor, const uint32_t request, const uint32_t arg0, const size_t arg1, const size_t arg2) {
    auto &filesystemService = getService<FilesystemService>();

    const auto &descriptor = filesystemService.getFileDescriptor(fileDescriptor);
    if (!descriptor.isValid()) {
        return -1;
    }

    return descriptor.getNode().control(request, arg0, arg1, arg2);
}

int64_t FilesystemService::systemCallControlFileDescriptor(const int32_t fileDescriptor, const uint32_t request, const uint32_t arg0, const size_t arg1, const size_t arg2) {
    auto &filesystemService = getService<FilesystemService>();

    return filesystemService.getFileDescriptor(fileDescriptor).control(request, arg0, arg1, arg2);
}

int64_t FilesystemService::systemCallChangeDirectory(const char *path) {
    auto &processService = getService<ProcessService>();

    return processService.getCurrentProcess().setWorkingDirectory(path) ? 0 : -1;
}

int64_t FilesystemService::systemCallGetCurrentWorkingDirectory() {
    auto &processService = getService<ProcessService>();
    auto &memoryService = getService<MemoryService>();

    const auto path = processService.getCurrentProcess().getWorkingDirectory().getCanonicalPath();
    auto *targetPath = static_cast<char *>(memoryService.allocateUserMemory((path.length() + 1) * sizeof(char)));

    const auto source = Util::Address(static_cast<const char*>(path));
    const auto target = Util::Address(targetPath);
    target.copyString(source);

    return reinterpret_cast<int64_t>(targetPath);
}

}
