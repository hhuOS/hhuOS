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

#include "lib/interface.h"
#include "device/cpu/Cpu.h"
#include "kernel/process/Process.h"
#include "kernel/system/System.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/FilesystemService.h"
#include "kernel/service/TimeService.h"
#include "kernel/service/PowerManagementService.h"

void *allocateMemory(uint32_t size, uint32_t alignment) {
    if (Kernel::System::isInitialized()) {
        return Kernel::System::getService<Kernel::MemoryService>().allocateKernelMemory(size, alignment);
    } else {
        return Kernel::System::allocateEarlyMemory(size);
    }
}

void* reallocateMemory(void *pointer, uint32_t size, uint32_t alignment) {
    return Kernel::System::getService<Kernel::MemoryService>().reallocateKernelMemory(pointer, size, alignment);
}

void freeMemory(void *pointer, uint32_t alignment) {
    if (Kernel::System::isInitialized()) {
        Kernel::System::getService<Kernel::MemoryService>().freeKernelMemory(pointer, alignment);
    } else {
        Kernel::System::freeEarlyMemory(pointer);
    }
}

bool isSystemInitialized() {
    return Kernel::System::isInitialized();
}

void* mapIO(uint32_t physicalAddress, uint32_t size) {
    return Kernel::System::getService<Kernel::MemoryService>().mapIO(physicalAddress, size);
}

void unmap(uint32_t virtualStartAddress, uint32_t virtualEndAddress, uint32_t breakCount) {
    Kernel::System::getService<Kernel::MemoryService>().unmap(virtualStartAddress, virtualEndAddress, breakCount);
}

bool createFile(const Util::Memory::String &path, Util::File::Type type) {
    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
    if (type == Util::File::REGULAR) {
        return filesystem.createFile(path);
    } else if (type == Util::File::DIRECTORY) {
        return filesystem.createDirectory(path);
    }

    return false;
}

bool deleteFile(const Util::Memory::String &path) {
    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
    return filesystem.deleteFile(path);
}

int32_t openFile(const Util::Memory::String &path) {
    return Kernel::System::getService<Kernel::FilesystemService>().openFile(path);
}

void closeFile(int32_t fileDescriptor) {
    Kernel::System::getService<Kernel::FilesystemService>().closeFile(fileDescriptor);
}

Util::File::Type getFileType(int32_t fileDescriptor) {
    return Kernel::System::getService<Kernel::FilesystemService>().getNode(fileDescriptor).getFileType();
}

uint32_t getFileLength(int32_t fileDescriptor) {
    return Kernel::System::getService<Kernel::FilesystemService>().getNode(fileDescriptor).getLength();
}

Util::Data::Array<Util::Memory::String> getFileChildren(int32_t fileDescriptor) {
    return Kernel::System::getService<Kernel::FilesystemService>().getNode(fileDescriptor).getChildren();
}

uint64_t readFile(int32_t fileDescriptor, uint8_t *targetBuffer, uint64_t pos, uint64_t length) {
    return Kernel::System::getService<Kernel::FilesystemService>().getNode(fileDescriptor).readData(targetBuffer, pos, length);
}

uint64_t writeFile(int32_t fileDescriptor, const uint8_t *sourceBuffer, uint64_t pos, uint64_t length) {
    return Kernel::System::getService<Kernel::FilesystemService>().getNode(fileDescriptor).writeData(sourceBuffer, pos, length);
}

bool changeDirectory(const Util::Memory::String &path) {
    return Kernel::System::getService<Kernel::SchedulerService>().getCurrentProcess().setWorkingDirectory(path);
}

Util::File::File getCurrentWorkingDirectory() {
    return Kernel::System::getService<Kernel::SchedulerService>().getCurrentProcess().getWorkingDirectory();
}

Util::Async::Process executeBinary(const Util::File::File &binaryFile, const Util::File::File &outputFile, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments) {
    auto &process = Kernel::System::getService<Kernel::SchedulerService>().loadBinary(binaryFile, outputFile, command, arguments);
    return Util::Async::Process(process.getId());
}

Util::Async::Process getCurrentProcess() {
    auto &process = Kernel::System::getService<Kernel::SchedulerService>().getCurrentProcess();
    return Util::Async::Process(process.getId());
}

Util::Async::Thread getCurrentThread() {
    auto &thread = Kernel::System::getService<Kernel::SchedulerService>().getCurrentThread();
    return Util::Async::Thread(thread.getId());
}

bool isProcessActive(uint32_t id) {
    return Kernel::System::getService<Kernel::SchedulerService>().isProcessActive(id);
}

void joinProcess(uint32_t id) {
    auto *process = Kernel::System::getService<Kernel::SchedulerService>().getProcess(id);
    if (process != nullptr) {
        process->join();
    }
}

Util::Time::Timestamp getSystemTime() {
    return Kernel::System::getService<Kernel::TimeService>().getSystemTime();
}

Util::Time::Date getCurrentDate() {
    return Kernel::System::getService<Kernel::TimeService>().getCurrentDate();
}

void setDate(const Util::Time::Date &date) {
    Kernel::System::getService<Kernel::TimeService>().setCurrentDate(date);
}

bool shutdown(Util::Machine::ShutdownType type) {
    auto &powerManagementService = Kernel::System::getService<Kernel::PowerManagementService>();

    if (type == Util::Machine::SHUTDOWN) {
        powerManagementService.shutdownMachine();
    } else if (type == Util::Machine::REBOOT) {
        powerManagementService.rebootMachine();
    }

    // If this code is reached, the shutdown was not successful
    return false;
}

void throwError(Util::Exception::Error error, const char *message){
    Device::Cpu::throwException(error, message);
}
