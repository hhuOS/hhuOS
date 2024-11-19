/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <stdint.h>

#include "lib/interface.h"
#include "kernel/process/Process.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/FilesystemService.h"
#include "kernel/service/TimeService.h"
#include "kernel/service/PowerManagementService.h"
#include "kernel/service/ProcessService.h"
#include "filesystem/Node.h"
#include "kernel/process/Thread.h"
#include "kernel/service/NetworkService.h"
#include "kernel/network/Socket.h"
#include "lib/util/base/Address.h"
#include "lib/util/network/Datagram.h"
#include "lib/util/async/Process.h"
#include "lib/util/async/Thread.h"
#include "lib/util/base/Exception.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/hardware/Machine.h"
#include "lib/util/io/file/File.h"
#include "lib/util/network/Socket.h"
#include "lib/util/time/Date.h"
#include "lib/util/time/Timestamp.h"
#include "GatesOfHell.h"
#include "lib/util/base/System.h"
#include "kernel/log/Log.h"
#include "kernel/service/Service.h"
#include "lib/util/base/HeapMemoryManager.h"
#include "lib/util/io/stream/PrintStream.h"
#include "kernel/process/Scheduler.h"
#include "kernel/memory/MemoryLayout.h"
#include "device/cpu/Cpu.h"
#include "kernel/process/FileDescriptor.h"
#include "kernel/service/InformationService.h"

namespace Util {
namespace Async {
class Runnable;
}  // namespace Async
}  // namespace Util

void *allocateMemory(uint32_t size, uint32_t alignment) {
    return GatesOfHell::getKernelHeap().allocateMemory(size, alignment);
}

void* reallocateMemory(void *pointer, uint32_t size, uint32_t alignment) {
    if (pointer == nullptr) {
        return allocateMemory(size, alignment);
    }

    return GatesOfHell::getKernelHeap().reallocateMemory(pointer, size, alignment);
}

void freeMemory(void *pointer, uint32_t alignment) {
    return GatesOfHell::getKernelHeap().freeMemory(pointer, alignment);
}

bool isMemoryManagementInitialized() {
    return Kernel::Service::isServiceRegistered(Kernel::MemoryService::SERVICE_ID);
}

void* mapIO(void *physicalAddress, uint32_t pageCount) {
    return Kernel::Service::getService<Kernel::MemoryService>().mapIO(physicalAddress, pageCount, false);
}

void unmap(void *virtualAddress, uint32_t pageCount, uint32_t breakCount) {
    Kernel::Service::getService<Kernel::MemoryService>().unmap(virtualAddress, pageCount, breakCount);
}

bool mount(const Util::String &deviceName, const Util::String &targetPath, const Util::String &driverName) {
    return Kernel::Service::getService<Kernel::FilesystemService>().mount(deviceName, targetPath, driverName);
}

bool unmount(const Util::String &path) {
    return Kernel::Service::getService<Kernel::FilesystemService>().unmount(path);
}

bool createFile(const Util::String &path, Util::Io::File::Type type) {
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    if (type == Util::Io::File::REGULAR) {
        return filesystemService.createFile(path);
    } else if (type == Util::Io::File::DIRECTORY) {
        return filesystemService.createDirectory(path);
    }

    return false;
}

bool deleteFile(const Util::String &path) {
    return Kernel::Service::getService<Kernel::FilesystemService>().deleteFile(path);
}

int32_t openFile(const Util::String &path) {
    return Kernel::Service::getService<Kernel::FilesystemService>().openFile(path);
}

void closeFile(int32_t fileDescriptor) {
    Kernel::Service::getService<Kernel::FilesystemService>().closeFile(fileDescriptor);
}

Util::Io::File::Type getFileType(int32_t fileDescriptor) {
    return Kernel::Service::getService<Kernel::FilesystemService>().getFileDescriptor(fileDescriptor).getNode().getType();
}

uint32_t getFileLength(int32_t fileDescriptor) {
    return Kernel::Service::getService<Kernel::FilesystemService>().getFileDescriptor(fileDescriptor).getNode().getLength();
}

Util::Array<Util::String> getFileChildren(int32_t fileDescriptor) {
    return Kernel::Service::getService<Kernel::FilesystemService>().getFileDescriptor(fileDescriptor).getNode().getChildren();
}

uint64_t readFile(int32_t fileDescriptor, uint8_t *targetBuffer, uint64_t pos, uint64_t length) {
    uint32_t read = 0;

    auto &descriptor = Kernel::Service::getService<Kernel::FilesystemService>().getFileDescriptor(fileDescriptor);
    if (descriptor.getAccessMode() == Util::Io::File::BLOCKING || descriptor.getNode().isReadyToRead()) {
        read = descriptor.getNode().readData(targetBuffer, pos, length);
    }

    return read;
}

uint64_t writeFile(int32_t fileDescriptor, const uint8_t *sourceBuffer, uint64_t pos, uint64_t length) {
    return Kernel::Service::getService<Kernel::FilesystemService>().getFileDescriptor(fileDescriptor).getNode().writeData(sourceBuffer, pos, length);
}

bool controlFile(int32_t fileDescriptor, uint32_t request, const Util::Array<uint32_t> &parameters) {
    return Kernel::Service::getService<Kernel::FilesystemService>().getFileDescriptor(fileDescriptor).getNode().control(request, parameters);
}

bool controlFileDescriptor(int32_t fileDescriptor, uint32_t request, const Util::Array<uint32_t> &parameters) {
    return Kernel::Service::getService<Kernel::FilesystemService>().getFileDescriptor(fileDescriptor).control(request, parameters);
}

bool changeDirectory(const Util::String &path) {
    return Kernel::Service::getService<Kernel::ProcessService>().getCurrentProcess().setWorkingDirectory(path);
}

Util::Io::File getCurrentWorkingDirectory() {
    return Kernel::Service::getService<Kernel::ProcessService>().getCurrentProcess().getWorkingDirectory();
}

int32_t createSocket(Util::Network::Socket::Type socketType) {
    return Kernel::Service::getService<Kernel::NetworkService>().createSocket(socketType);
}

bool sendDatagram(int32_t fileDescriptor, const Util::Network::Datagram &datagram) {
    auto &socket = reinterpret_cast<Kernel::Network::Socket&>(Kernel::Service::getService<Kernel::FilesystemService>().getFileDescriptor(fileDescriptor).getNode());
    return socket.send(datagram);
}

bool receiveDatagram(int32_t fileDescriptor, Util::Network::Datagram &datagram) {
    auto &socket = reinterpret_cast<Kernel::Network::Socket&>(Kernel::Service::getService<Kernel::FilesystemService>().getFileDescriptor(fileDescriptor).getNode());
    auto *kernelDatagram = socket.receive();
    if (kernelDatagram == nullptr) {
        return false;
    }

    auto *datagramBuffer = new uint8_t[kernelDatagram->getLength()];

    auto source = Util::Address<uint32_t>(kernelDatagram->getData());
    auto target = Util::Address<uint32_t>(datagramBuffer);
    target.copyRange(source, kernelDatagram->getLength());

    datagram.setData(datagramBuffer, kernelDatagram->getLength());
    datagram.setRemoteAddress(kernelDatagram->getRemoteAddress());
    datagram.setAttributes(*kernelDatagram);

    delete kernelDatagram;
    return true;
}

Util::Async::Process executeBinary(const Util::Io::File &binaryFile, const Util::Io::File &inputFile, const Util::Io::File &outputFile, const Util::Io::File &errorFile, const Util::String &command, const Util::Array<Util::String> &arguments) {
    auto &process = Kernel::Service::getService<Kernel::ProcessService>().loadBinary(binaryFile, inputFile, outputFile, errorFile, command, arguments);
    return Util::Async::Process(process.getId());
}

Util::Async::Process getCurrentProcess() {
    auto &process = Kernel::Service::getService<Kernel::ProcessService>().getCurrentProcess();
    return Util::Async::Process(process.getId());
}
Util::Async::Thread createThread(const Util::String &name, Util::Async::Runnable *runnable) {
    auto &thread = Kernel::Thread::createKernelThread(name, Kernel::Service::getService<Kernel::ProcessService>().getKernelProcess(), runnable);
    Kernel::Service::getService<Kernel::ProcessService>().getScheduler().ready(thread);
    return Util::Async::Thread(thread.getId());
}

Util::Async::Thread getCurrentThread() {
    auto &thread = Kernel::Service::getService<Kernel::ProcessService>().getScheduler().getCurrentThread();
    return Util::Async::Thread(thread.getId());
}

void joinThread(uint32_t id) {
    auto *thread = Kernel::Service::getService<Kernel::ProcessService>().getScheduler().getThread(id);
    if (thread != nullptr) {
        thread->join();
    }
}

void joinProcess(uint32_t id) {
    auto *process = Kernel::Service::getService<Kernel::ProcessService>().getProcess(id);
    if (process != nullptr) {
        process->join();
    }
}

void killProcess(uint32_t id) {
    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    auto *process = processService.getProcess(id);
    if (process != nullptr) {
        processService.killProcess(*process);
    }
}

void sleep(const Util::Time::Timestamp &time) {
    if (isSchedulerInitialized()) {
        Kernel::Service::getService<Kernel::ProcessService>().getScheduler().sleep(time);
    } else {
        Kernel::Service::getService<Kernel::TimeService>().busyWait(time);
    }
}

void yield() {
    Kernel::Service::getService<Kernel::ProcessService>().getScheduler().yield();
}

bool isSchedulerInitialized() {
    return Kernel::Service::getService<Kernel::ProcessService>().getScheduler().isInitialized();
}

Util::Time::Timestamp getSystemTime() {
    return Kernel::Service::isServiceRegistered(Kernel::TimeService::SERVICE_ID) ? Kernel::Service::getService<Kernel::TimeService>().getSystemTime() : Util::Time::Timestamp::ofMilliseconds(0);
}

Util::Time::Date getCurrentDate() {
    return Kernel::Service::getService<Kernel::TimeService>().getCurrentDate();
}

void setDate(const Util::Time::Date &date) {
    Kernel::Service::getService<Kernel::TimeService>().setCurrentDate(date);
}

bool shutdown(Util::Hardware::Machine::ShutdownType type) {
    auto &powerManagementService = Kernel::Service::getService<Kernel::PowerManagementService>();

    if (type == Util::Hardware::Machine::SHUTDOWN) {
        powerManagementService.shutdownMachine();
    } else if (type == Util::Hardware::Machine::REBOOT) {
        powerManagementService.rebootMachine();
    }

    // If this code is reached, the shutdown was not successful
    return false;
}

void printKernelStackTrace(bool log) {
    uint32_t *ebp = nullptr;
    asm volatile (
            "mov %%ebp, (%0);"
            : :
            "r"(&ebp)
            :
            "eax"
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
            Util::System::out << Util::String::format("0x%08x", eip) << " " << symbolName << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        }

        ebp = reinterpret_cast<uint32_t*>(ebp[0]);
    }
}

void throwError(Util::Exception::Error error, const char *message) {
    if (Kernel::Service::isServiceRegistered(Kernel::ProcessService::SERVICE_ID) && Kernel::Service::getService<Kernel::ProcessService>().getScheduler().isInitialized()) {
        auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
        if (processService.getCurrentProcess().isKernelProcess()) {
            Device::Cpu::disableInterrupts();

            Util::System::out << "Kernel Panic: " << Util::Exception::getExceptionName(error) << " (" << message <<  ")" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            printKernelStackTrace(false);

            Util::System::out << "System halt!" << Util::Io::PrintStream::flush;
            Device::Cpu::halt();
        } else {
            Util::System::out << Util::Exception::getExceptionName(error) << " (" << message <<  ")" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            printKernelStackTrace(false);
            processService.exitCurrentProcess(-1);
        }

    } else {
        LOG_ERROR("Kernel Panic: %s (%s)", Util::Exception::getExceptionName(error), message);
        printKernelStackTrace(true);

        Device::Cpu::disableInterrupts();
        Device::Cpu::halt();
    }
}
