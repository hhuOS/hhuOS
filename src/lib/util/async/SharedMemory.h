/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LIB_UTIL_ASYNC_SHAREDMEMORY_H
#define HHUOS_LIB_UTIL_ASYNC_SHAREDMEMORY_H

#include <stddef.h>

#include "util/async/Process.h"
#include "util/base/String.h"

namespace Util {
namespace Async {

/// Allows the creation and mapping of shared memory regions.
/// Such regions are created by one process and can be mapped into the address space of other processes.
/// Any changes to the memory region are visible to all processes that have mapped the region.
///
/// Typically, one process creates a new shared memory regions and then calls `publish()`
/// to make it available to other processes. After that, the created region is visible in the file system
/// under `/process/<pid>/shared/<name>`, where `<pid>` is the process id and `<name>` must be a unique identifier
/// given in the constructor of this class.
/// Other processes can then open the file and issue a `controlFile()` request with `Request::MAP`
/// to map the region into their address space. This is simplified by creating a `SharedMemory` object
/// with the constructor that takes a `Process` object and a name. Calling `map()` on that object
/// will open the file and issue the request.
///
/// ## Example (publisher process):
/// ```c++
/// // Create a new shared memory region named "myregion" with a size of 1 page (4096 bytes).
/// const auto sharedMemory = Util::Async::SharedMemory("myregion", 1);
///
/// // Copy a string into the shared memory region.
/// const auto address = sharedMemory.getAddress();
/// address.copyString("Hello, Shared Memory!");
///
/// // Publish the region to make it available to other processes.
/// if (!sharedMemory.publish()) {
///     Util::System::out << "Failed to publish shared memory region!"
///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
///     return;
/// }
/// ```
///
/// ## Example (mapping process):
/// ```c++
/// // The PID of the publisher process must be known.
/// // It can, for example, be communicated via pipes.
/// // This example assumes that the PID is 42.
/// const auto id = 42;
///
/// const auto sharedMemory = Util::Async::SharedMemory(id, "myregion", 1);
/// if (!sharedMemory.map()) {
///     Util::System::out << "Failed to map shared memory region!"
///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
///     return;
/// }
///
/// const auto address = sharedMemory.getAddress();
/// const auto message = static_cast<const char*>(address.getAsPointer());
/// Util::System::out << "Message from shared memory: " << message
///     << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // Should print "Hello, Shared Memory!"
/// ```
class SharedMemory {

public:
    /// Requests that can be issued to the file that represents the shared memory region.
    /// This is typically not done directly by applications, since all requests are wrapped in methods of this class.
    enum Request {
        MAP
    };

    /// Create a new shared memory region with the given name and size (in pages).
    /// This constructor only allocates the region on the heap, but does not publish it.
    /// To make the region available to other processes, `publish()` must be called.
    SharedMemory(const String &name, size_t pageCount);

    /// Open an existing shared memory region with the given name in the given process.
    /// This constructor only allocates space for the region on the heap, but does not map it.
    /// To map the region into the address space of the current process, `map()` must be called.
    SharedMemory(size_t processId, const String &name, size_t pageCount);

    /// SharedMemory is not copyable, since it contains a pointer to a memory region.
    /// Multiple instances pointing to the same region would cause double frees.
    SharedMemory(const SharedMemory &other) = delete;

    /// SharedMemory is not copyable, since it contains a pointer to a memory region.
    /// Multiple instances pointing to the same region would cause double frees.
    SharedMemory &operator=(const SharedMemory &other) = delete;

    /// Destroy the shared memory region by freeing the allocated memory.
    /// This does not unmap the region from other processes that have mapped it.
    ~SharedMemory();

    /// Get the address of the shared memory region.
    const Address& getAddress() const {
        return address;
    }

    /// Publish the shared memory region to make it available to other processes.
    /// This can only be called by the process that created the region.
    /// Returns true on success, false on failure.
    bool publish() const;

    /// Map the shared memory region into the address space of the current process.
    /// This can only be called by processes other than the one that created the region.
    /// Returns true on success, false on failure.
    bool map() const;

private:

    Process process;
    String name;
    Address address;
    size_t pageCount;
};

}
}

#endif