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
#include "kernel/service/ProcessService.h"
#include "filesystem/core/Node.h"
#include "kernel/process/Thread.h"
#include "kernel/service/SchedulerService.h"
#include "kernel/service/NetworkService.h"
#include "kernel/network/Socket.h"
#include "lib/util/memory/Address.h"
#include "lib/util/network/Datagram.h"

namespace Util {
namespace Async {
class Runnable;
}  // namespace Async
}  // namespace Util

extern uint32_t scheduler_initialized;

void *allocateMemory(uint32_t size, uint32_t alignment) {
    if (Kernel::System::isInitialized()) {
        return Kernel::System::getService<Kernel::MemoryService>().allocateKernelMemory(size, alignment);
    } else {
        return Kernel::System::allocateEarlyMemory(size);
    }
}

void* reallocateMemory(void *pointer, uint32_t size, uint32_t alignment) {
    if (pointer == nullptr) {
        return allocateMemory(size, alignment);
    }

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
    return Kernel::System::getService<Kernel::MemoryService>().mapIO(physicalAddress, size, false);
}

void unmap(uint32_t virtualStartAddress, uint32_t virtualEndAddress, uint32_t breakCount) {
    Kernel::System::getService<Kernel::MemoryService>().unmap(virtualStartAddress, virtualEndAddress, breakCount);
}

bool mount(const Util::Memory::String &deviceName, const Util::Memory::String &targetPath, const Util::Memory::String &driverName) {
    return Kernel::System::getService<Kernel::FilesystemService>().mount(deviceName, targetPath, driverName);
}

bool unmount(const Util::Memory::String &path) {
    return Kernel::System::getService<Kernel::FilesystemService>().unmount(path);
}

bool createFile(const Util::Memory::String &path, Util::File::Type type) {
    auto &filesystemService = Kernel::System::getService<Kernel::FilesystemService>();
    if (type == Util::File::REGULAR) {
        return filesystemService.createFile(path);
    } else if (type == Util::File::DIRECTORY) {
        return filesystemService.createDirectory(path);
    }

    return false;
}

bool deleteFile(const Util::Memory::String &path) {
    return Kernel::System::getService<Kernel::FilesystemService>().deleteFile(path);
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

bool controlFile(int32_t fileDescriptor, uint32_t request, const Util::Data::Array<uint32_t> &parameters) {
    return Kernel::System::getService<Kernel::FilesystemService>().getNode(fileDescriptor).control(request, parameters);
}

bool changeDirectory(const Util::Memory::String &path) {
    return Kernel::System::getService<Kernel::ProcessService>().getCurrentProcess().setWorkingDirectory(path);
}

Util::File::File getCurrentWorkingDirectory() {
    return Kernel::System::getService<Kernel::ProcessService>().getCurrentProcess().getWorkingDirectory();
}

int32_t createSocket(Util::Network::Socket::Type socketType) {
    return Kernel::System::getService<Kernel::NetworkService>().createSocket(socketType);
}

bool sendDatagram(int32_t fileDescriptor, const Util::Network::Datagram &datagram) {
    auto &socket = reinterpret_cast<Kernel::Network::Socket&>(Kernel::System::getService<Kernel::FilesystemService>().getNode(fileDescriptor));
    return socket.send(datagram);
}

bool receiveDatagram(int32_t fileDescriptor, Util::Network::Datagram &datagram) {
    auto &socket = reinterpret_cast<Kernel::Network::Socket&>(Kernel::System::getService<Kernel::FilesystemService>().getNode(fileDescriptor));
    auto *kernelDatagram = socket.receive();
    auto *datagramBuffer = new uint8_t[kernelDatagram->getLength()];

    auto source = Util::Memory::Address<uint32_t>(kernelDatagram->getData());
    auto target = Util::Memory::Address<uint32_t>(datagramBuffer);
    target.copyRange(source, kernelDatagram->getLength());

    datagram.setData(datagramBuffer, kernelDatagram->getLength());
    datagram.setRemoteAddress(kernelDatagram->getRemoteAddress());
    datagram.setAttributes(*kernelDatagram);

    delete kernelDatagram;
    return true;
}

Util::Async::Process executeBinary(const Util::File::File &binaryFile, const Util::File::File &inputFile, const Util::File::File &outputFile, const Util::File::File &errorFile, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments) {
    auto &process = Kernel::System::getService<Kernel::ProcessService>().loadBinary(binaryFile, inputFile, outputFile, errorFile, command, arguments);
    return Util::Async::Process(process.getId());
}

Util::Async::Process getCurrentProcess() {
    auto &process = Kernel::System::getService<Kernel::ProcessService>().getCurrentProcess();
    return Util::Async::Process(process.getId());
}
Util::Async::Thread createThread(const Util::Memory::String &name, Util::Async::Runnable *runnable) {
    auto &thread = Kernel::Thread::createKernelThread(name, Kernel::System::getService<Kernel::ProcessService>().getKernelProcess(), runnable);
    Kernel::System::getService<Kernel::SchedulerService>().ready(thread);
    return Util::Async::Thread(thread.getId());
}

Util::Async::Thread getCurrentThread() {
    auto &thread = Kernel::System::getService<Kernel::SchedulerService>().getCurrentThread();
    return Util::Async::Thread(thread.getId());
}

void joinThread(uint32_t id) {
    auto *thread = Kernel::System::getService<Kernel::SchedulerService>().getThread(id);
    if (thread != nullptr) {
        thread->join();
    }
}

void joinProcess(uint32_t id) {
    auto *process = Kernel::System::getService<Kernel::ProcessService>().getProcess(id);
    if (process != nullptr) {
        process->join();
    }
}

void killProcess(uint32_t id) {
    auto &processService = Kernel::System::getService<Kernel::ProcessService>();
    auto *process = processService.getProcess(id);
    if (process != nullptr) {
        processService.killProcess(*process);
    }
}

void sleep(const Util::Time::Timestamp &time) {
    if (scheduler_initialized) {
        Kernel::System::getService<Kernel::SchedulerService>().sleep(time);
    } else {
        Kernel::System::getService<Kernel::TimeService>().busyWait(time);
    }
}

void yield() {
    Kernel::System::getService<Kernel::SchedulerService>().yield();
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

void throwError(Util::Exception::Error error, const char *message) {
    Device::Cpu::throwException(error, message);
}
