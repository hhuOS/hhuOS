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

#include "kernel/system/System.h"
#include "FilesystemService.h"

namespace Kernel {

FilesystemService::FilesystemService() {
    SystemCall::registerSystemCall(Util::System::OPEN_FILE, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 2) {
            return Util::System::INVALID_ARGUMENT;
        }

        const char *path = va_arg(arguments, const char*);
        int32_t *fileDescriptor = va_arg(arguments, int32_t*);

        *fileDescriptor = System::getService<FilesystemService>().openFile(path);
        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::CLOSE_FILE, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 1) {
            return Util::System::INVALID_ARGUMENT;
        }

        int32_t fileDescriptor = va_arg(arguments, int32_t);

        System::getService<FilesystemService>().closeFile(fileDescriptor);
        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::CREATE_FILE, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 2) {
            return Util::System::INVALID_ARGUMENT;
        }

        const char *path = va_arg(arguments, const char*);
        auto type = static_cast<Util::File::Type>(va_arg(arguments, uint32_t));

        if (type != Util::File::REGULAR && type != Util::File::DIRECTORY) {
            return Util::System::INVALID_ARGUMENT;
        }

        auto success = type == Util::File::REGULAR ? System::getService<FilesystemService>().getFilesystem().createFile(path) :
                System::getService<FilesystemService>().getFilesystem().createDirectory(path);
        return success ? Util::System::Result::OK : Util::System::Result::INVALID_ARGUMENT;
    });

    SystemCall::registerSystemCall(Util::System::DELETE_FILE, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 1) {
            return Util::System::INVALID_ARGUMENT;
        }

        const char *path = va_arg(arguments, const char*);

        return System::getService<FilesystemService>().getFilesystem().deleteFile(path) ? Util::System::Result::OK : Util::System::Result::INVALID_ARGUMENT;
    });

    SystemCall::registerSystemCall(Util::System::FILE_TYPE, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 2) {
            return Util::System::INVALID_ARGUMENT;
        }

        int32_t fileDescriptor = va_arg(arguments, int32_t);
        Util::File::Type *type = va_arg(arguments, Util::File::Type*);

        *type = System::getService<FilesystemService>().getNode(fileDescriptor).getFileType();
        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::FILE_LENGTH, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 2) {
            return Util::System::INVALID_ARGUMENT;
        }

        int32_t fileDescriptor = va_arg(arguments, int32_t);
        uint64_t *length = va_arg(arguments, uint64_t*);

        *length = System::getService<FilesystemService>().getNode(fileDescriptor).getLength();
        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::FILE_CHILDREN, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 3) {
            return Util::System::INVALID_ARGUMENT;
        }

        int32_t fileDescriptor = va_arg(arguments, int32_t);
        char ***targetChildren = va_arg(arguments, char***);
        uint32_t *count = va_arg(arguments, uint32_t*);

        auto &memoryService = System::getService<MemoryService>();
        auto children = System::getService<FilesystemService>().getNode(fileDescriptor).getChildren();
        *count = children.length();
        *targetChildren = static_cast<char**>(memoryService.allocateUserMemory(children.length() * sizeof(char*)));

        for (uint32_t i = 0; i < children.length(); i++) {
            (*targetChildren)[i] = static_cast<char*>(memoryService.allocateUserMemory(children[i].length() + 1));
            auto source = Util::Memory::Address<uint32_t>(static_cast<char*>(children[i]));
            auto target = Util::Memory::Address<uint32_t>((*targetChildren)[i]);
            target.copyString(source);
        }

        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::WRITE_FILE, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 5) {
            return Util::System::INVALID_ARGUMENT;
        }

        int32_t fileDescriptor = va_arg(arguments, int32_t);
        uint8_t *sourceBuffer = va_arg(arguments, uint8_t*);
        uint64_t pos = va_arg(arguments, uint64_t);
        uint64_t length = va_arg(arguments, uint64_t);
        uint64_t *written = va_arg(arguments, uint64_t*);

        *written = System::getService<FilesystemService>().getNode(fileDescriptor).writeData(sourceBuffer, pos, length);
        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::READ_FILE, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 5) {
            return Util::System::INVALID_ARGUMENT;
        }

        int32_t fileDescriptor = va_arg(arguments, int32_t);
        uint8_t *targetBuffer = va_arg(arguments, uint8_t*);
        uint64_t pos = va_arg(arguments, uint64_t);
        uint64_t length = va_arg(arguments, uint64_t);
        uint64_t *read = va_arg(arguments, uint64_t*);

        *read = System::getService<FilesystemService>().getNode(fileDescriptor).readData(targetBuffer, pos, length);
        return Util::System::Result::OK;
    });
}

Filesystem::Filesystem &FilesystemService::getFilesystem() {
    return filesystem;
}

int32_t FilesystemService::openFile(const Util::Memory::String &path) {
    return System::getService<SchedulerService>().getCurrentProcess().getFileDescriptorManager().openFile(path);
}

void FilesystemService::closeFile(int32_t fileDescriptor) {
    return System::getService<SchedulerService>().getCurrentProcess().getFileDescriptorManager().closeFile(fileDescriptor);
}

Filesystem::Node &FilesystemService::getNode(int32_t fileDescriptor) {
    return System::getService<SchedulerService>().getCurrentProcess().getFileDescriptorManager().getNode(fileDescriptor);
}

}