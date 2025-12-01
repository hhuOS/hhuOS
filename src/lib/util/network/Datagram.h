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
 *
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#ifndef HHUOS_LIB_UTIL_NETWORK_DATAGRAM_H
#define HHUOS_LIB_UTIL_NETWORK_DATAGRAM_H

#include <stdint.h>

#include "util/network/NetworkAddress.h"

namespace Util {
namespace Io {
class ByteArrayOutputStream;
}  // namespace Io
}  // namespace Util

namespace Util::Network {

/// Base class for network datagrams, which are used to send and receive data over a network.
/// A datagram contains a buffer with the data, its length, and the remote address to
/// which it is sent or from which it is received.
/// Subclasses must implement the `setAttributes()` method to copy type dependent attributes
/// from another datagram.
class Datagram {

public:
    /// Create a new datagram with a specified address type.
    /// The buffer remains uninitialized and must be set later using `setData()`.
    /// This is typically used for receiving datagrams, because in this case the buffer is allocated
    /// by the kernel during the receive system call.
    /// This class cannot be instantiated directly, but the constructor must be called by subclasses.
    explicit Datagram(NetworkAddress::Type type);

    /// Create a new datagram with a given buffer and length, and a remote address.
    /// The buffer's content is copied into the datagram's buffer.
    /// This class cannot be instantiated directly, but the constructor must be called by subclasses.
    Datagram(const uint8_t *buffer, uint16_t length, const NetworkAddress &remoteAddress);

    /// Create a new datagram from a byte array output stream and a remote address.
    /// The stream's content is copied into the datagram's buffer by directly accessing the stream's buffer.
    /// This way, the stream's state remains unchanged.
    /// This class cannot be instantiated directly, but the constructor must be called by subclasses.
    Datagram(const Io::ByteArrayOutputStream &stream, const NetworkAddress &remoteAddress);

    /// Datagrams are not copyable, so the copy constructor is deleted.
    Datagram(const Datagram &other) = delete;

    /// Datagrams are not copyable, so the assignment operator is deleted.
    Datagram &operator=(const Datagram &other) = delete;

    /// Destroy the datagram, freeing the allocated buffer and remote address.
    virtual ~Datagram();

    /// Get the remote address to which this datagram is sent or from which it is received.
    const NetworkAddress& getRemoteAddress() const;

    /// Set the remote address for this datagram.
    void setRemoteAddress(const NetworkAddress& address) const;

    /// Get access to the data buffer of this datagram.
    const uint8_t* getData() const;

    /// Get the length of the data buffer in bytes.
    /// When accessing the data buffer (using `getData()`),
    /// make sure to not read more bytes than the length of the buffer.
    /// Otherwise, this will lead to undefined behavior.
    uint32_t getLength() const;

    /// Set the data buffer for this datagram.
    /// This method will NOT copy the data, but simply set the pointer to the given buffer
    /// and free the previous buffer.
    /// This should be used with care, as the datagram will take ownership of the buffer,
    /// freeing it when the datagram is destroyed.
    void setData(uint8_t *buffer, uint32_t length);

    /// Set type dependent attributes from another datagram.
    /// This is used by the kernel to copy attributes from a kernel space datagram to a user space datagram.
    ///
    /// ### Example (from `EthernetDatagram`)
    /// ```c++
    /// void setAttributes(const Datagram &datagram) override {
    ///     auto &ethernetDatagram = reinterpret_cast<const EthernetDatagram&>(datagram); // Cast to specific type
    ///     type = ethernetDatagram.getEtherType(); // Set ether type (the only attribute for Ethernet datagrams)
    /// }
    /// ```
    virtual void setAttributes(const Datagram &datagram) = 0;

private:

    NetworkAddress *remoteAddress = nullptr;

    uint8_t *buffer = nullptr;
    uint32_t length = 0;
};

}

#endif
