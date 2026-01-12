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

/// \file interface.h
/// This file declares the interface functions provided by the OS kernel to user space programs.
/// Most of these functions are thin wrappers around kernel system calls.
/// User programs usually do not call these functions directly, but use higher-level abstractions provided
/// by the `util` library.
/// All of these functions are implemented twice, once in the kernel (in kernel.cpp)
/// and once in the user space library (user.cpp). This way, the `util` library code can be agnostic
/// of whether it is compiled for kernel and user space, with the correct implementation being
/// linked in at build time.

#ifndef HHUOS_LIB_INTERFACE_H
#define HHUOS_LIB_INTERFACE_H

#include <stddef.h>

#include "util/async/Process.h"
#include "util/async/Thread.h"
#include "util/base/Panic.h"
#include "util/base/String.h"
#include "util/collection/Array.h"
#include "util/hardware/Machine.h"
#include "util/io/file/File.h"
#include "util/network/Socket.h"
#include "util/time/Date.h"

/// Allocate a block of memory of the given size and alignment.
/// If no sufficient block of memory is available, a panic is fired.
void* allocateMemory(size_t size, size_t alignment = 0);

/// Reallocate a previously allocated block of memory to a new size and alignment.
/// Depending on the implementation, the current block may be extended in place.
/// Otherwise, a new block is allocated and the content of the old block is copied into the new one.
/// The old block is then freed.
/// If no sufficient block of memory is available, a panic is fired.
void* reallocateMemory(void *pointer, size_t size, size_t alignment = 0);

/// Free a block of memory that was previously allocated by `allocateMemory()` or `reallocateMemory()`.
void freeMemory(void *pointer, size_t alignment = 0);

/// Check if the memory management subsystem has been initialized.
/// For user space programs, this always returns true.
bool isMemoryManagementInitialized();

/// Map a physical memory region into the virtual address space of the calling process.
/// The region starts at the given physical address and spans the given number of pages.
/// The virtual memory region is allocated on the heap of the calling process and a pointer to its start is returned.
void* mapIO(size_t physicalAddress, size_t pageCount);

/// Unmap a previously mapped virtual memory region from the address space of the calling process.
/// The region starts at the given virtual address and spans the given number of pages.
/// The region is only unmapped; if it was allocated on the heap, the heap memory must be freed separately.
///
/// The optional `breakCount` parameter is used as a workaround for the `FreeListMemoryManager`
/// and usually not needed for other user code:
/// The memory manager automatically unmaps freed memory regions that are larger than a page.
/// If a chunk of memory is freed, that is adjacent to another chunk that was already unmapped,
/// the memory manager cannot detect this and will try to unmap the same region twice.
/// As a result, it may try to unmap very large blocks of memory that were never mapped in the first place.
/// The kernel ignores such unmap requests, but they waste time nonetheless.
/// To avoid this issue, the `breakCount` parameter can be used to tell the kernel to stop unmapping
/// after it has encountered the given number of unmapped pages in a row.
void unmap(void *virtualAddress, size_t pageCount, size_t breakCount = 0);

/// Mount a filesystem driver at the given target path, using the specified device and driver.
///
/// ### Example
/// ```c++
/// mount("/dev/hdd1p1", "/mnt", "Filesystem::Fat::FatDriver");
/// ```
bool mount(const Util::String &deviceName, const Util::String &targetPath, const Util::String &driverName);

/// Unmount the filesystem mounted at the given path.
/// Return true on success, false otherwise.
bool unmount(const Util::String &path);

/// Create a file or directory at the given path, depending on the specified type.
/// Return true on success, false otherwise.
bool createFile(const Util::String &path, Util::Io::File::Type type);

/// Delete the file or directory at the given path.
/// Return true on success, false otherwise.
bool deleteFile(const Util::String &path);

/// Open the file at the given path and return its file descriptor.
/// On error, -1 is returned.
int32_t openFile(const Util::String &path);

/// Close the given file descriptor.
/// After this call, the file descriptor is no longer valid and must not be used.
void closeFile(int32_t fileDescriptor);

/// Get the type of the file associated with the given file descriptor (e.g., regular file, directory, character file).
Util::Io::File::Type getFileType(int32_t fileDescriptor);

/// Get the length of the file associated with the given file descriptor in bytes.
size_t getFileLength(int32_t fileDescriptor);

/// Get the names of the children of the directory associated with the given file descriptor.
/// The names are returned as an array of strings.
/// If the file descriptor does not refer to a directory, an empty array is returned.
Util::Array<Util::String> getFileChildren(int32_t fileDescriptor);

/// Read data from the file associated with the given file descriptor into the target buffer.
/// The read starts at the given position in the file and reads up to the specified length in bytes.
/// The actual number of bytes read is returned.
uint64_t readFile(int32_t fileDescriptor, uint8_t *targetBuffer, uint64_t pos, uint64_t length);

/// Write data from the source buffer to the file associated with the given file descriptor.
/// The write starts at the given position in the file and writes up to the specified length in bytes.
/// The actual number of bytes written is returned.
uint64_t writeFile(int32_t fileDescriptor, const uint8_t *sourceBuffer, uint64_t pos, uint64_t length);

/// Issue a control request to the file associated with the given file descriptor.
/// The request is specified by the request code and the parameters.
/// This can for example be used to manipulate devices via files they expose.
/// The request and parameters are specific to the device driver.
/// Return true on success, false otherwise.
bool controlFile(int32_t fileDescriptor, size_t request, const Util::Array<size_t> &parameters);

/// Issue a control request to the file descriptor itself.
/// This can for example be used to change the access mode of the file descriptor (blocking or non-blocking)
/// or to check if the file descriptor is ready to read.
/// Return true on success, false otherwise.
bool controlFileDescriptor(int32_t fileDescriptor, size_t request, const Util::Array<size_t> &parameters);

/// Change the current working directory of the calling process to the given path.
/// Return true on success, false otherwise.
bool changeDirectory(const Util::String &path);

/// Get the current working directory of the calling process as a `Util::Io::File` object.
Util::Io::File getCurrentWorkingDirectory();

/// Create a named pipe with the given name in the file system.
/// The pipe is created in the path "/proc/<current_process_id>/pipes/<name>".
/// Return true on success, false otherwise.
bool createPipe(const Util::String &name);

/// Create a shared memory region with the given name, starting at the specified address,
/// spanning the given number of pages.
/// The region is accessible by other processes via a file in the path "/proc/<current_process_id>/shm/<name>".
/// Return true on success, false otherwise.
bool createSharedMemory(const Util::String &name, void *startAddress, size_t pageCount);

/// Create a network socket of the specified type (e.g., UDP).
/// Return the file descriptor of the created socket, or -1 on error.
int32_t createSocket(Util::Network::Socket::Type socketType);

/// Send a datagram via the socket associated with the given file descriptor.
/// Return true on success, false otherwise.
bool sendDatagram(int32_t fileDescriptor, const Util::Network::Datagram &datagram);

/// Receive a datagram via the socket associated with the given file descriptor.
/// The received datagram is stored in the provided `datagram` object.
/// Depending on the socket's configuration, this call may block until a datagram is available.
/// Return true on success, false otherwise.
bool receiveDatagram(int32_t fileDescriptor, Util::Network::Datagram &datagram);

/// Execute a binary file in a new process with the given input, output, and error files, command, and arguments.
/// Return a `Util::Async::Process` object representing the created process.
Util::Async::Process executeBinary(const Util::Io::File &binaryFile, const Util::Io::File &inputFile, const Util::Io::File &outputFile, const Util::Io::File &errorFile, const Util::String &command, const Util::Array<Util::String> &arguments);

/// Get a `Util::Async::Process` object representing the currently running process.
Util::Async::Process getCurrentProcess();

/// Create a new thread with the given name that runs the specified `Util::Async::Runnable` object.
/// Return a `Util::Async::Thread` object representing the created thread.
Util::Async::Thread createThread(const Util::String &name, Util::Async::Runnable *runnable);

/// Get a `Util::Async::Thread` object representing the currently running thread.
Util::Async::Thread getCurrentThread();

/// Join the thread with the given ID, blocking until it terminates.
void joinThread(size_t id);

/// Join the process with the given ID, blocking until it terminates.
/// This will just join the main thread of the process.
void joinProcess(size_t id);

/// Kill the process with the given ID.
void killProcess(size_t id);

/// Put the current thread to sleep for the specified duration.
void sleep(const Util::Time::Timestamp &time);

/// Yield the CPU to allow other threads to run.
void yield();

/// Check if the scheduler has been initialized.
/// For user space programs, this always returns true.
bool isSchedulerInitialized();

/// Get the current system time as a `Util::Time::Timestamp` object.
Util::Time::Timestamp getSystemTime();

/// Get the current system date as a `Util::Time::Date` object.
Util::Time::Date getCurrentDate();

/// Set the current system date to the specified date.
void setDate(const Util::Time::Date &date);

/// Shutdown or reboot the machine, depending on the specified shutdown type.
bool shutdown(Util::Hardware::Machine::ShutdownType type);

/// Print the given error message and a stack trace, then terminate the program.
/// This is used by `Util::Panic` to print panic messages.
/// In kernel space, this function does not return and halts the system.
[[noreturn]] void throwError(Util::Panic::Error error, const char *message);

#endif
