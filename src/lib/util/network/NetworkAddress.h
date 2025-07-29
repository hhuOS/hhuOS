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

#ifndef HHUOS_LIB_UTIL_NETWOR_NETWORKADDRESS_H
#define HHUOS_LIB_UTIL_NETWOR_NETWORKADDRESS_H

#include <stdint.h>

#include "base/String.h"

namespace Util {
namespace Io {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network {

/// Base class for network addresses, providing a common interface for different types of network addresses,
/// such as MAC or IP addresses.
/// A network address consists of a buffer that holds the address data, its length, and its type.
/// This class manages the address data and provides methods for reading/writing the address from/to streams,
/// overwriting the address data and comparing addresses with each other.
/// Subclasses must implement the trivial method `createCopy()` to return a copy of the address
/// and the `toString()` method to return a string representation of the address.
class NetworkAddress {

public:
    /// Different types of network addresses.
    enum Type {
        /// MAC address, typically tied to a hardware interface and six bytes long.
        MAC,
        /// IPv4 address, typically used in networking and four bytes long.
        IP4,
        /// IPv4 address with an additional 16-bit port number to identify a specific application on the host.
        IP4_PORT,
        /// IPv4 subnet address, used to identify a range of IP addresses within a network.
        IP4_SUBNET
    };

    /// Create a new network address with a specified length and type.
    /// The buffer is zeroed out after allocation.
    /// This class cannot be instantiated directly, but the constructor must be called by subclasses.
    NetworkAddress(uint8_t length, Type type);

    /// Create a new network address from a given buffer.
    /// The buffer's content is copied into the address buffer.
    /// This class cannot be instantiated directly, but the constructor must be called by subclasses.
    NetworkAddress(const uint8_t *buffer, uint8_t length, Type type);

    /// Create a new network address from an existing one.
    /// The content of the existing address is copied into the new address buffer.
    NetworkAddress(const NetworkAddress &other);

    /// Copy the content of one network address to another.
    /// The content of the source address is copied into the destination address buffer.
    NetworkAddress &operator=(const NetworkAddress &other);

    /// Destroy the network address, freeing the allocated buffer.
    virtual ~NetworkAddress();

    /// Compare the network address with another address for equality.
    bool operator==(const NetworkAddress &other) const;

    /// Compare the network address with another address for inequality.
    bool operator!=(const NetworkAddress &other) const;

    /// Read the address from an input stream, overwriting the existing address data.
    /// This method will block until enough data is available to fill the address buffer.
    void read(Io::InputStream &stream) const;

    /// Write the address data to an output stream.
    void write(Io::OutputStream &stream) const;

    /// Overwrite the address data with the content of the given buffer.
    /// The buffer must be at least as long as the address length.
    /// If the buffer is shorter, bytes will be read out of bounds, leading to undefined behavior.
    /// If the buffer is longer, only the first `length` bytes are copied.
    void setAddress(const uint8_t *buffer) const;

    /// Copy the address data into the given buffer.
    /// The buffer must be at least as long as the address length.
    /// If the buffer is shorter, bytes will be written out of bounds, leading to a buffer overflow.
    /// If the buffer is longer, only the first `length` bytes are copied.
    void getAddress(uint8_t *buffer) const;

    /// Return the address length in bytes.
    [[nodiscard]] uint8_t getLength() const;

    /// Return the address type.
    [[nodiscard]] Type getType() const;

    /// Compare the address with another address bit by bit.
    /// If the addresses are equal, 0 is returned.
    /// Otherwise, the index of the first differing bit is returned.
    /// This is for example useful to calculate a subnet prefix length.
    [[nodiscard]] uint8_t compareTo(const NetworkAddress &other) const;

    /// Create a new on-heap instance of the address as a copy of this address.
    /// This method is trivial, but must be implemented by subclasses.
    /// It cannot be implemented in this base class because virtual classes cannot be instantiated directly.
    ///
    /// ### Example (from `MacAddress`)
    /// ```c++
    /// NetworkAddress* createCopy() const override {
    ///     return new MacAddress(*this);
    /// }
    /// ```
    [[nodiscard]] virtual NetworkAddress* createCopy() const = 0;

    /// Create a string representation of the address.
    /// This may look different depending on the address type.
    [[nodiscard]] virtual String toString() const = 0;

protected:

    /// The buffer that holds the address data.
    uint8_t *buffer;
    /// The length of the address buffer in bytes.
    uint8_t length;

private:

    Type type;
};

}

#endif
