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

#ifndef HHUOS_INTERFACE_H
#define HHUOS_INTERFACE_H

#include <cstdint>

#include "lib/util/Exception.h"
#include "lib/util/file/File.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/time/Date.h"
#include "lib/util/async/Process.h"
#include "lib/util/system/Machine.h"
#include "lib/util/async/Thread.h"
#include "lib/util/data/Array.h"
#include "lib/util/file/Type.h"
#include "lib/util/memory/String.h"
#include "lib/util/network/Socket.h"

namespace Util {
namespace Network {
class Datagram;
}  // namespace Network

namespace Async {
class Runnable;
}  // namespace Async
}  // namespace Util

void* allocateMemory(uint32_t size, uint32_t alignment = 0);
void* reallocateMemory(void *pointer, uint32_t size, uint32_t alignment = 0);
void freeMemory(void *pointer, uint32_t alignment = 0);

bool isSystemInitialized();
void* mapIO(uint32_t physicalAddress, uint32_t size);
void unmap(uint32_t virtualStartAddress, uint32_t virtualEndAddress, uint32_t breakCount = 0);

bool mount(const Util::Memory::String &deviceName, const Util::Memory::String &targetPath, const Util::Memory::String &driverName);
bool unmount(const Util::Memory::String &path);
bool createFile(const Util::Memory::String &path, Util::File::Type type);
bool deleteFile(const Util::Memory::String &path);
int32_t openFile(const Util::Memory::String &path);
void closeFile(int32_t fileDescriptor);
Util::File::Type getFileType(int32_t fileDescriptor);
uint32_t getFileLength(int32_t fileDescriptor);
Util::Data::Array<Util::Memory::String> getFileChildren(int32_t fileDescriptor);
uint64_t readFile(int32_t fileDescriptor, uint8_t *targetBuffer, uint64_t pos, uint64_t length);
uint64_t writeFile(int32_t fileDescriptor, const uint8_t *sourceBuffer, uint64_t pos, uint64_t length);
bool controlFile(int32_t fileDescriptor, uint32_t request, const Util::Data::Array<uint32_t> &parameters);
bool changeDirectory(const Util::Memory::String &path);
Util::File::File getCurrentWorkingDirectory();

int32_t createSocket(Util::Network::Socket::Type socketType);
bool sendDatagram(int32_t fileDescriptor, const Util::Network::Datagram &datagram);
bool receiveDatagram(int32_t fileDescriptor, Util::Network::Datagram &datagram);

Util::Async::Process executeBinary(const Util::File::File &binaryFile, const Util::File::File &inputFile, const Util::File::File &outputFile, const Util::File::File &errorFile, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments);
Util::Async::Process getCurrentProcess();
Util::Async::Thread createThread(const Util::Memory::String &name, Util::Async::Runnable *runnable);
Util::Async::Thread getCurrentThread();
void joinThread(uint32_t id);
void joinProcess(uint32_t id);
void killProcess(uint32_t id);
void sleep(const Util::Time::Timestamp &time);
void yield();

Util::Time::Timestamp getSystemTime();
Util::Time::Date getCurrentDate();
void setDate(const Util::Time::Date &date);

bool shutdown(Util::Machine::ShutdownType type);
[[noreturn]] void throwError(Util::Exception::Error error, const char *message);

#endif
