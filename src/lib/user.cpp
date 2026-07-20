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

#include "interface.h"

#include "util/async/Process.h"
#include "util/async/Runnable.h"
#include "util/async/Thread.h"
#include "util/base/Constants.h"
#include "util/base/FreeListMemoryManager.h"
#include "util/base/Panic.h"
#include "util/base/String.h"
#include "util/base/System.h"
#include "util/collection/Array.h"
#include "util/hardware/Machine.h"
#include "util/io/file/File.h"
#include "util/io/stream/PrintStream.h"
#include "util/network/Socket.h"
#include "util/network/ethernet/EthernetDatagram.h"
#include "util/network/icmp/IcmpDatagram.h"
#include "util/network/ip4/Ip4Datagram.h"
#include "util/network/udp/UdpDatagram.h"
#include "util/time/Date.h"
#include "util/time/Timestamp.h"

void* allocateMemory(const size_t size, const size_t alignment) {
    return Util::System::getAddressSpaceHeader().heapMemoryManager.allocateMemory(size, alignment);
}

void* reallocateMemory(void *pointer, const size_t size, const size_t alignment) {
    if (pointer == nullptr) {
        return allocateMemory(size, alignment);
    }

    return Util::System::getAddressSpaceHeader().heapMemoryManager.reallocateMemory(pointer, size, alignment);
}

void freeMemory(void *pointer, const size_t alignment) {
    Util::System::getAddressSpaceHeader().heapMemoryManager.freeMemory(pointer, alignment);
}

bool isMemoryManagementInitialized() {
    return true;
}

void* mapIO(const size_t physicalAddress, const size_t pageCount) {
    auto *mappedAddress = Util::System::call<void*>(Util::System::MAP_IO, 3, physicalAddress, pageCount);

    return mappedAddress;
}

bool unmap(void *virtualAddress, const size_t pageCount, const size_t breakCount) {
    return Util::System::call(Util::System::UNMAP, 3, virtualAddress, pageCount, breakCount) == 0;
}

bool mount(const Util::String &deviceName, const Util::String &targetPath, const Util::String &driverName) {
    return Util::System::call(Util::System::MOUNT, 3,static_cast<const char*>(deviceName),
        static_cast<const char*>(targetPath), static_cast<const char*>(driverName)) == 0;
}

bool unmount(const Util::String &path) {
    return Util::System::call(Util::System::UNMOUNT, 1, static_cast<const char*>(path)) == 0;
}

bool createFile(const Util::String &path, const Util::Io::File::Type type) {
    return Util::System::call(Util::System::CREATE_FILE, 2, static_cast<const char*>(path), type) == 0;
}

bool deleteFile(const Util::String &path) {
    return Util::System::call(Util::System::DELETE_FILE, 1, static_cast<const char*>(path)) == 0;
}

int32_t openFile(const Util::String &path) {
    return Util::System::call<int32_t>(Util::System::OPEN_FILE, 1, static_cast<const char*>(path));
}

void closeFile(const int32_t fileDescriptor) {
    Util::System::call(Util::System::CLOSE_FILE, 1, fileDescriptor);
}

Util::Io::File::Type getFileType(const int32_t fileDescriptor) {
    const auto type = Util::System::call<int32_t>(Util::System::FILE_TYPE, 1, fileDescriptor);
    if (type == -1) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Failed to query file type!");
    }

    return static_cast<Util::Io::File::Type>(type);
}

uint64_t getFileLength(const int32_t fileDescriptor) {
    const int64_t length = Util::System::call(Util::System::FILE_LENGTH, 1, fileDescriptor);
    if (length == -1) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Failed to query file length!");
    }

    return length;
}

Util::Array<Util::String> getFileChildren(const int32_t fileDescriptor) {
    const char **children;
    const auto count = Util::System::call(Util::System::FILE_CHILDREN, 2, fileDescriptor,
        &children);

    auto ret = Util::Array<Util::String>(count);
    for (size_t i = 0; i < count; i++) {
        ret[i] = children[i];
        delete children[i];
    }

    delete children;
    return ret;
}

uint64_t readFile(const int32_t fileDescriptor, uint8_t *targetBuffer, const uint64_t pos, const uint64_t length) {
    const auto read = Util::System::call(Util::System::READ_FILE, 5, fileDescriptor,
        targetBuffer, static_cast<uint32_t>(pos), static_cast<uint32_t>(pos >> 32), length);

    return read > 0 ? read : 0;
}

uint64_t writeFile(const int32_t fileDescriptor, const uint8_t *sourceBuffer, const uint64_t pos,
    const uint32_t length)
{
    const auto written = Util::System::call(Util::System::WRITE_FILE, 5, fileDescriptor,
        sourceBuffer, static_cast<uint32_t>(pos), static_cast<uint32_t>(pos >> 32), length);

    return written > 0 ? written : 0;
}

int64_t controlFile(const int32_t fileDescriptor, const size_t request, const size_t arg0, const size_t arg1,
    const size_t arg2)
{
    return Util::System::call(Util::System::CONTROL_FILE, 5, fileDescriptor, request, arg0, arg1, arg2);
}

bool controlFileDescriptor(const int32_t fileDescriptor, const size_t request, const size_t arg0, const size_t arg1,
    const size_t arg2)
{
    return Util::System::call(Util::System::CONTROL_FILE_DESCRIPTOR, 5, fileDescriptor, request,
        arg0, arg1, arg2) == 0;
}

bool changeDirectory(const Util::String &path) {
    return Util::System::call(Util::System::CHANGE_DIRECTORY, 1, static_cast<const char*>(path)) == 0;
}

Util::Io::File getCurrentWorkingDirectory() {
    const auto *path = reinterpret_cast<char*>(Util::System::call(
        Util::System::GET_CURRENT_WORKING_DIRECTORY, 0));

    const auto file = Util::Io::File(path);
    delete path;

    return file;
}

bool createPipe(const Util::String &name) {
    return Util::System::call(Util::System::CREATE_PIPE, 1, static_cast<const char*>(name)) == 0;
}

bool createSharedMemory(const Util::String &name, void *startAddress, const size_t pageCount) {
    return Util::System::call(Util::System::CREATE_SHARED_MEMORY, 3,
        static_cast<const char*>(name), startAddress, pageCount) == 0;
}

int32_t createSocket(const Util::Network::Socket::Type socketType) {
    return Util::System::call<int32_t>(Util::System::CREATE_SOCKET, 1, socketType);
}

bool sendDatagram(const int32_t fileDescriptor, const Util::Network::Socket::Type socketType,
    const Util::Network::Datagram &datagram)
{
    uint32_t datagramArg = 0;
    switch (socketType) {
        case Util::Network::Socket::ETHERNET: {
            const auto &ethernetDatagram = reinterpret_cast<const Util::Network::Ethernet::EthernetDatagram&>(datagram);
            datagramArg = ethernetDatagram.getEtherType();
        }
        break;
        case Util::Network::Socket::IP4: {
            const auto &ip4Datagram = reinterpret_cast<const Util::Network::Ip4::Ip4Datagram&>(datagram);
            datagramArg = ip4Datagram.getProtocol();
        }
        break;
        case Util::Network::Socket::ICMP: {
            const auto &icmpDatagram = reinterpret_cast<const Util::Network::Icmp::IcmpDatagram&>(datagram);
            datagramArg = (icmpDatagram.getCode() << 8) | static_cast<uint32_t>(icmpDatagram.getType());
        }
        break;
        case Util::Network::Socket::UDP:
            break;
        case Util::Network::Socket::IP6:
        case Util::Network::Socket::TCP:
            return false;
    }

    const auto *payload = datagram.getData();
    const auto length = datagram.getLength();
    const auto *addressBuffer = datagram.getRemoteAddress().getBuffer();

    return Util::System::call(Util::System::SEND_DATAGRAM, 5, fileDescriptor, addressBuffer, payload,
        length, datagramArg) == 0;
}

const Util::Network::Datagram* receiveDatagram(const int32_t fileDescriptor,
    const Util::Network::Socket::Type socketType)
{
    uint8_t *remoteAddressBuffer;
    uint8_t *payload;
    uint32_t length;
    uint32_t datagramArg;

    const auto success = Util::System::call(Util::System::RECEIVE_DATAGRAM, 5, fileDescriptor,
        &remoteAddressBuffer, &payload, &length, &datagramArg);

    if (success < 0) {
        return nullptr;
    }

    switch (socketType) {
        case Util::Network::Socket::ETHERNET:
            return new Util::Network::Ethernet::EthernetDatagram(payload, length,
                Util::Network::MacAddress(remoteAddressBuffer),
                static_cast<Util::Network::Ethernet::EthernetHeader::EtherType>(datagramArg));
        case Util::Network::Socket::IP4:
            return new Util::Network::Ip4::Ip4Datagram(payload, length,
                Util::Network::Ip4::Ip4Address(remoteAddressBuffer),
                static_cast<Util::Network::Ip4::Ip4Header::Protocol>(datagramArg));
        case Util::Network::Socket::ICMP:
            return new Util::Network::Icmp::IcmpDatagram(payload, length,
                Util::Network::Ip4::Ip4Address(remoteAddressBuffer),
                static_cast<Util::Network::Icmp::IcmpHeader::Type>((datagramArg >> 8) & 0xff), datagramArg & 0xff);
        case Util::Network::Socket::UDP:
            return new Util::Network::Udp::UdpDatagram(payload, length,
                Util::Network::Ip4::Ip4PortAddress(remoteAddressBuffer));
        case Util::Network::Socket::TCP:
        case Util::Network::Socket::IP6:
        default:
            return nullptr;
    }
}

Util::Async::Process executeBinary(const Util::Io::File &binaryFile, const Util::Io::File &inputFile,
    const Util::Io::File &outputFile, const Util::Io::File &errorFile, const Util::String &command,
    const Util::Array<Util::String> &arguments)
{
    const auto binaryPath = binaryFile.getCanonicalPath();
    const auto inputPath = inputFile.getCanonicalPath();
    const auto outputPath = outputFile.getCanonicalPath();
    const auto errorPath = errorFile.getCanonicalPath();

    const char *stdPaths[3] {
        static_cast<const char*>(inputPath),
        static_cast<const char*>(outputPath),
        static_cast<const char*>(errorPath)
    };

    const auto argc = arguments.length();
    const auto **argv = new const char*[argc];
    for (size_t i = 0; i < argc; i++) {
        argv[i] = static_cast<const char*>(arguments[i]);
    }

    const auto processId = Util::System::call(Util::System::EXECUTE_BINARY, 5,
        static_cast<const char*>(binaryPath), stdPaths, static_cast<const char*>(command), argc, argv);

    return Util::Async::Process(processId);
}

Util::Async::Process getCurrentProcess() {
    const auto processId = Util::System::call(Util::System::GET_CURRENT_PROCESS, 0);

    return Util::Async::Process(processId);
}

void kickoffUserThread(Util::Async::Runnable *runnable) {
    runnable->run();

    delete runnable;
    Util::System::call(Util::System::EXIT_THREAD, 0);
}

Util::Async::Thread createThread(const Util::String &name, Util::Async::Runnable *runnable) {
    const auto threadId = Util::System::call(Util::System::CREATE_THREAD, 4,
        static_cast<const char*>(name), runnable, kickoffUserThread);

    return Util::Async::Thread(threadId);
}

Util::Async::Thread getCurrentThread() {
    const auto threadId = Util::System::call(Util::System::GET_CURRENT_THREAD, 0);

    return Util::Async::Thread(threadId);
}

void joinThread(const size_t id) {
    Util::System::call(Util::System::JOIN_THREAD, 1, id);
}

void joinProcess(const size_t id) {
    Util::System::call(Util::System::JOIN_PROCESS, 1, id);
}

void killProcess(const size_t id) {
    Util::System::call(Util::System::KILL_PROCESS, 1, id);
}

void sleep(const Util::Time::Timestamp &time) {
    Util::System::call(Util::System::SLEEP, 1, &time);
}

void yield() {
    Util::System::call(Util::System::YIELD, 0);
}

bool isSchedulerInitialized() {
    return true;
}

Util::Time::Timestamp getSystemTime() {
    const auto time = Util::System::call(Util::System::GET_SYSTEM_TIME, 0);

    return Util::Time::Timestamp::ofNanoseconds(time);
}

Util::Time::Date getCurrentDate() {
    const int64_t unixTime = Util::System::call(Util::System::GET_CURRENT_DATE, 0);

    return Util::Time::Date(unixTime);
}

bool setDate(const Util::Time::Date &date) {
    return Util::System::call(Util::System::SET_DATE, 1, &date) == 0;
}

bool shutdown(const Util::Hardware::Machine::ShutdownType type) {
    return Util::System::call(Util::System::SHUTDOWN, 1, type);
}

void throwError(const Util::Panic::Error error, const char *message) {
    Util::System::out << Util::Panic::getErrorAsString(error) <<
        " (" << message << ")" << Util::Io::PrintStream::lnFlush;
    Util::System::printStackTrace(Util::System::out, Util::USER_SPACE_MEMORY_START_ADDRESS);

    Util::System::call(Util::System::EXIT_PROCESS, 1, -1);
    __builtin_unreachable();
}