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

#include <stddef.h>
#include <stdint.h>

#include "interface.h"

#include "util/async/Process.h"
#include "util/async/Thread.h"
#include "util/base/Address.h"
#include "util/base/HeapMemoryManager.h"
#include "util/base/Panic.h"
#include "util/base/String.h"
#include "util/base/System.h"
#include "util/collection/Array.h"
#include "util/network/Datagram.h"
#include "util/hardware/Machine.h"
#include "util/io/file/File.h"
#include "util/io/stream/PrintStream.h"
#include "util/network/Socket.h"
#include "util/time/Date.h"
#include "util/time/Timestamp.h"
#include "kernel/log/Log.h"
#include "kernel/memory/MemoryLayout.h"
#include "kernel/network/Socket.h"
#include "kernel/process/FileDescriptor.h"
#include "kernel/process/Process.h"
#include "kernel/process/Scheduler.h"
#include "kernel/process/Thread.h"
#include "kernel/service/Service.h"
#include "kernel/service/FilesystemService.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/TimeService.h"
#include "kernel/service/PowerManagementService.h"
#include "kernel/service/ProcessService.h"
#include "kernel/service/NetworkService.h"
#include "kernel/service/InformationService.h"
#include "filesystem/Node.h"
#include "device/cpu/Cpu.h"
#include "GatesOfHell.h"

void* allocateMemory(const size_t size, const size_t alignment) {
    return GatesOfHell::getKernelHeap().allocateMemory(size, alignment);
}

void* reallocateMemory(void *pointer, const size_t size, const size_t alignment) {
    if (pointer == nullptr) {
        return allocateMemory(size, alignment);
    }

    return GatesOfHell::getKernelHeap().reallocateMemory(pointer, size, alignment);
}

void freeMemory(void *pointer, const size_t alignment) {
    return GatesOfHell::getKernelHeap().freeMemory(pointer, alignment);
}

bool isMemoryManagementInitialized() {
    return Kernel::Service::isServiceRegistered(Kernel::MemoryService::SERVICE_ID);
}

void* mapIO(const size_t physicalAddress, const size_t pageCount) {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    return memoryService.mapIO(reinterpret_cast<void*>(physicalAddress), pageCount, false);
}

void unmap(void *virtualAddress, const size_t pageCount, const size_t breakCount) {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    memoryService.unmap(virtualAddress, pageCount, breakCount);
}

bool mount(const Util::String &deviceName, const Util::String &targetPath, const Util::String &driverName) {
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    return filesystemService.mount(deviceName, targetPath, driverName);
}

bool unmount(const Util::String &path) {
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    return filesystemService.unmount(path);
}

bool createFile(const Util::String &path, const Util::Io::File::Type type) {
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();

    if (type == Util::Io::File::REGULAR) {
        return filesystemService.createFile(path);
    }
    if (type == Util::Io::File::DIRECTORY) {
        return filesystemService.createDirectory(path);
    }

    return false;
}

bool deleteFile(const Util::String &path) {
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    return filesystemService.deleteFile(path);
}

int32_t openFile(const Util::String &path) {
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    return filesystemService.openFile(path);
}

void closeFile(const int32_t fileDescriptor) {
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    filesystemService.closeFile(fileDescriptor);
}

Util::Io::File::Type getFileType(const int32_t fileDescriptor) {
    if (!Kernel::Service::isServiceRegistered(Kernel::FilesystemService::SERVICE_ID)) {
        switch (fileDescriptor) {
            case Util::Io::STANDARD_INPUT:
            case Util::Io::STANDARD_OUTPUT:
            case Util::Io::STANDARD_ERROR:
                return Util::Io::File::CHARACTER;
            default:
                Util::Panic::fire(Util::Panic::ILLEGAL_STATE,
                    "Trying to access filesystem before it is initialized");
        }
    }

    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    return filesystemService.getFileDescriptor(fileDescriptor).getNode().getType();
}

size_t getFileLength(const int32_t fileDescriptor) {
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    return filesystemService.getFileDescriptor(fileDescriptor).getNode().getLength();
}

Util::Array<Util::String> getFileChildren(const int32_t fileDescriptor) {
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    return filesystemService.getFileDescriptor(fileDescriptor).getNode().getChildren();
}

uint64_t readFile(const int32_t fileDescriptor, uint8_t *targetBuffer, const uint64_t pos, const uint64_t length) {
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    const auto &descriptor = filesystemService.getFileDescriptor(fileDescriptor);

    uint64_t read = 0;
    if (descriptor.getAccessMode() == Util::Io::File::BLOCKING || descriptor.getNode().isReadyToRead()) {
        read = descriptor.getNode().readData(targetBuffer, pos, length);
    }

    return read;
}

uint64_t writeFile(const int32_t fileDescriptor, const uint8_t *sourceBuffer, const uint64_t pos,
    const uint64_t length)
{
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    return filesystemService.getFileDescriptor(fileDescriptor).getNode().writeData(sourceBuffer, pos, length);
}

bool controlFile(const int32_t fileDescriptor, const size_t request, const Util::Array<size_t> &parameters) {
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    return filesystemService.getFileDescriptor(fileDescriptor).getNode().control(request, parameters);
}

bool controlFileDescriptor(const int32_t fileDescriptor, const size_t request,
    const Util::Array<size_t> &parameters)
{
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    return filesystemService.getFileDescriptor(fileDescriptor).control(request, parameters);
}

bool changeDirectory(const Util::String &path) {
    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    return processService.getCurrentProcess().setWorkingDirectory(path);
}

Util::Io::File getCurrentWorkingDirectory() {
    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    return processService.getCurrentProcess().getWorkingDirectory();
}

bool createPipe(const Util::String &name) {
    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    return processService.getCurrentProcess().createPipe(name);
}

bool createSharedMemory(const Util::String &name, void *startAddress, const size_t pageCount) {
    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    return processService.getCurrentProcess().createSharedMemory(name, startAddress, pageCount);
}

int32_t createSocket(const Util::Network::Socket::Type socketType) {
    auto &networkService = Kernel::Service::getService<Kernel::NetworkService>();
    return networkService.createSocket(socketType);
}

bool sendDatagram(const int32_t fileDescriptor, const Util::Network::Datagram &datagram) {
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    auto &socketNode = filesystemService.getFileDescriptor(fileDescriptor).getNode();
    auto &socket = reinterpret_cast<Kernel::Network::Socket&>(socketNode);

    return socket.send(datagram);
}

bool receiveDatagram(const int32_t fileDescriptor, Util::Network::Datagram &datagram) {
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    auto &socketNode = filesystemService.getFileDescriptor(fileDescriptor).getNode();
    auto &socket = reinterpret_cast<Kernel::Network::Socket&>(socketNode);

    const auto *kernelDatagram = socket.receive();
    if (kernelDatagram == nullptr) {
        return false;
    }

    auto *datagramBuffer = new uint8_t[kernelDatagram->getLength()];

    const auto source = Util::Address(kernelDatagram->getData());
    const auto target = Util::Address(datagramBuffer);
    target.copyRange(source, kernelDatagram->getLength());

    datagram.setData(datagramBuffer, kernelDatagram->getLength());
    datagram.setRemoteAddress(kernelDatagram->getRemoteAddress());
    datagram.setAttributes(*kernelDatagram);

    delete kernelDatagram;
    return true;
}

Util::Async::Process executeBinary(const Util::Io::File &binaryFile, const Util::Io::File &inputFile,
    const Util::Io::File &outputFile, const Util::Io::File &errorFile, const Util::String &command,
    const Util::Array<Util::String> &arguments)
{
    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    const auto &process = processService.loadBinary(binaryFile, inputFile, outputFile, errorFile, command, arguments);

    return Util::Async::Process(process.getId());
}

Util::Async::Process getCurrentProcess() {
    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    const auto &process = processService.getCurrentProcess();

    return Util::Async::Process(process.getId());
}
Util::Async::Thread createThread(const Util::String &name, Util::Async::Runnable *runnable) {
    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    auto &thread = Kernel::Thread::createKernelThread(name, processService.getKernelProcess(), runnable);

    processService.getScheduler().ready(thread);
    return Util::Async::Thread(thread.getId());
}

Util::Async::Thread getCurrentThread() {
    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    const auto &thread = processService.getScheduler().getCurrentThread();

    return Util::Async::Thread(thread.getId());
}

void joinThread(const size_t id) {
    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    auto *thread = processService.getScheduler().getThread(id);
    if (thread != nullptr) {
        thread->join();
    }
}

void joinProcess(const size_t id) {
    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    auto *process = processService.getProcess(id);
    if (process != nullptr) {
        process->join();
    }
}

void killProcess(const size_t id) {
    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    auto *process = processService.getProcess(id);
    if (process != nullptr) {
        processService.killProcess(*process);
    }
}

void sleep(const Util::Time::Timestamp &time) {
    if (isSchedulerInitialized()) {
        auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
        processService.getScheduler().sleep(time);
    } else {
        const auto &timeService = Kernel::Service::getService<Kernel::TimeService>();
        timeService.busyWait(time);
    }
}

void yield() {
    if (Kernel::Service::isServiceRegistered(Kernel::ProcessService::SERVICE_ID)) {
        auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
        processService.getScheduler().yield();
    }
}

bool isSchedulerInitialized() {
    if (!Kernel::Service::isServiceRegistered(Kernel::ProcessService::SERVICE_ID)) {
        return false;
    }

    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    return processService.getScheduler().isInitialized();
}

Util::Time::Timestamp getSystemTime() {
    if (Kernel::Service::isServiceRegistered(Kernel::TimeService::SERVICE_ID)) {
        const auto &timeService = Kernel::Service::getService<Kernel::TimeService>();
        return timeService.getSystemTime();
    }

    return Util::Time::Timestamp();
}

Util::Time::Date getCurrentDate() {
    const auto &timeService = Kernel::Service::getService<Kernel::TimeService>();
    return timeService.getCurrentDate();
}

void setDate(const Util::Time::Date &date) {
    auto &timeService = Kernel::Service::getService<Kernel::TimeService>();
    timeService.setCurrentDate(date);
}

bool shutdown(const Util::Hardware::Machine::ShutdownType type) {
    auto &powerManagementService = Kernel::Service::getService<Kernel::PowerManagementService>();

    if (type == Util::Hardware::Machine::SHUTDOWN) {
        powerManagementService.shutdownMachine();
    } else if (type == Util::Hardware::Machine::REBOOT) {
        powerManagementService.rebootMachine();
    }

    // If this code is reached, the shutdown was not successful
    return false;
}

void printKernelStackTrace(const bool log) {
    uint32_t *ebp = nullptr;
    asm volatile (
            "mov %%ebp, %0;"
            : "=r"(ebp)
            );

    while (reinterpret_cast<uint32_t>(ebp) >= Kernel::MemoryLayout::KERNEL_START) {
        uint32_t eip = ebp[1];

        if (eip == 0x0000DEAD || eip < Kernel::MemoryLayout::KERNEL_START || eip > Kernel::MemoryLayout::KERNEL_END) {
            break;
        }

        const char *symbolName = "";
        if (Kernel::Service::isServiceRegistered(Kernel::InformationService::SERVICE_ID)) {
            uint32_t symbolEip = eip;
            auto &informationService = Kernel::Service::getService<Kernel::InformationService>();
            symbolName = informationService.getSymbolName(symbolEip);
            while (symbolName == nullptr && symbolEip >= Kernel::MemoryLayout::KERNEL_START) {
                symbolName = informationService.getSymbolName(--symbolEip);
            }
        }

        if (log) {
            LOG_ERROR("0x%08x %s", eip, symbolName);
        } else {
            Util::System::out << Util::String::format("0x%08x", eip) << " " << symbolName << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        }

        ebp = reinterpret_cast<uint32_t*>(ebp[0]);
    }
}

void throwErrorEarly(const Util::Panic::Error error, const char *message) {
    LOG_ERROR("Kernel Panic: %s (%s)", Util::Panic::getErrorAsString(error), message);
    printKernelStackTrace(true);

    Device::Cpu::halt();
}

void throwError(const Util::Panic::Error error, const char *message) {
    if (!Kernel::Service::isServiceRegistered(Kernel::ProcessService::SERVICE_ID)) {
        throwErrorEarly(error, message);
    }

    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    if (!processService.getScheduler().isInitialized()) {
        throwErrorEarly(error, message);
    }

    if (processService.getCurrentProcess().isKernelProcess()) {
        Device::Cpu::disableInterrupts();

        Util::System::out << "Kernel Panic: " << Util::Panic::getErrorAsString(error) <<
            " (" << message <<  ")" << Util::Io::PrintStream::lnFlush;
        printKernelStackTrace(false);

        Util::System::out << "System halt!" << Util::Io::PrintStream::flush;
        Device::Cpu::halt();
    }

    Util::System::out << Util::Panic::getErrorAsString(error) <<
        " (" << message <<  ")" << Util::Io::PrintStream::lnFlush;
    printKernelStackTrace(false);
    processService.exitCurrentProcess(-1);
}
