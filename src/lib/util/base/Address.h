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

#ifndef HHUOS_LIB_UTIL_ADDRESS_H
#define HHUOS_LIB_UTIL_ADDRESS_H

#include <stdint.h>
#include <stddef.h>

namespace Util {

/// Wraps a memory address and provides methods to manipulate the memory it points to.
class Address {

public:
    /// Create a new Address object pointing to the address 0.
    Address() = default;

    /// Create a new Address object pointing to the given address.
	explicit Address(void *pointer);

    /// Create a new Address object pointing to the given address.
    explicit Address(const void *pointer);

    /// Create a new Address object pointing to the given address.
    explicit Address(size_t address);

    /// Get the address this object points to.
    [[nodiscard]] size_t get() const;

    /// Get the address this object points to as a void pointer.
    [[nodiscard]] void* getAsPointer() const;

    /// Add a value to the address this object points to
    /// and return a new `Address` object pointing to the result.
    [[nodiscard]] Address add(size_t value) const;

    /// Subtract a value from the address this object points to
    /// and return a new `Address` object pointing to the result.
    [[nodiscard]] Address subtract(size_t value) const;

    /// Align the address this object points to up to the given alignment
    /// and return a new `Address` object pointing to the result.
    ///
    /// ### Example
    /// ```c++
    /// const auto aligned = Util::Address(0x1234).alignUp(0x1000); // aligned = 0x2000
    /// ```
    [[nodiscard]] Address alignUp(size_t alignment) const;

    /// Align the address this object points to down to the given alignment
    /// and return a new `Address` object pointing to the result.
    ///
    /// ### Example
    /// ```c++
    /// const auto aligned = Util::Address(0x1234).alignDown(0x1000); // aligned = 0x1000
    /// ```
    [[nodiscard]] Address alignDown(size_t alignment) const;

    /// Count the number of bytes until a null terminator (0) is reached.
    ///
    /// ### Example
    /// ```c++
    /// const char *str = "Hello, World!";
    /// const auto len = Util::Address(str).stringLength(); // len = 13
    /// ```
    [[nodiscard]] size_t stringLength() const;

    /// Compare the memory this object points to with the memory the given address points to.
    /// The number of bytes to compare is given by the `length` parameter.
    /// If the memory is equal, 0 is returned.
    ///
    /// ### Example
    /// ```c++
    /// const uint8_t a[4] = { 1, 2, 3, 4 };
    /// const uint8_t b[4] = { 5, 6, 7, 8 };
    /// const auto equal = Util::Address(a).compareRange(Util::Address(b), 4); // equal != 0
    /// ```
    [[nodiscard]] int32_t compareRange(const Address &otherAddress, size_t length) const;

    /// Compare the memory this object points to with the memory the given address points to.
    /// The comparison is done byte by byte until a null terminator (0) is reached on either side.
    /// If the memory is equal, 0 is returned.
    ///
    /// ### Example
    /// ```c++
    /// const char *a = "Hello";
    /// const char *b = "World!";
    /// const auto equal = Util::Address(a).compareString(Util::Address(b)); // equal != 0
    /// ```
    [[nodiscard]] int32_t compareString(const Address &otherAddress) const;

    /// Read a byte from the address this object points to plus the given offset.
    [[nodiscard]] uint8_t read8(size_t offset = 0) const;

    /// Read a 16-bit value from the address this object points to plus the given offset.
    [[nodiscard]] uint16_t read16(size_t offset = 0) const;

    /// Read a 32-bit value from the address this object points to plus the given offset.
    [[nodiscard]] uint32_t read32(size_t offset = 0) const;

    /// Read a 64-bit value from the address this object points to plus the given offset.
    [[nodiscard]] uint64_t read64(size_t offset = 0) const;

    /// Write a byte to the address this object points to plus the given offset.
    void write8(uint8_t value, size_t offset = 0) const;

    /// Write a 16-bit value to the address this object points to plus the given offset.
    void write16(uint16_t value, size_t offset = 0) const;

    /// Write a 32-bit value to the address this object points to plus the given offset.
    void write32(uint32_t value, size_t offset = 0) const;

    /// Write a 64-bit value to the address this object points to plus the given offset.
    void write64(uint64_t value, size_t offset = 0) const;

    /// Set the memory this object points to to the given value.
    /// The number of bytes to set is given by the `length` parameter.
    ///
    /// ### Example
    /// ```c++
    /// auto address = Util::Address(0x1234);
    /// address.setRange(0, 4); // Set 4 bytes to 0 (equivalent to memset((void*) 0x1234, 0, 4))
    /// ```
    void setRange(uint8_t value, size_t length) const;

    /// Copy the memory from the given address to the memory this object points to.
    /// The number of bytes to copy is given by the `length` parameter.
    ///
    /// ### Example
    /// ```c++
    /// auto source = Util::Address(0x1234);
    /// auto target = Util::Address(0x5678);
    ///
    /// // Copy 4 bytes from source to target (equivalent to memcpy((void*) 0x5678, (void*) 0x1234, 4))
    /// target.copyRange(source, 4);
    /// ```
    void copyRange(const Address &sourceAddress, size_t length) const;

    /// Copy the memory from the given address to the memory this object points to.
    /// Copying is done until a null terminator (0) is reached on the source. The null-terminator is also copied.
    ///
    /// ### Example
    /// ```c++
    /// auto source = "Hello, World!";
    /// char target[14] = {};
    ///
    /// // Equivalent to strcpy((char*) target, (char*) source)
    /// Util::Address(target).copyString(Util::Address(source));
    /// ```
    void copyString(const Address &sourceAddress) const;

    /// Copy the memory from the given string to the memory this object points to.
    /// Copying is done until a null terminator (0) is reached on the source. The null-terminator is also copied.
    ///
    /// ### Example
    /// ```c++
    /// auto source = "Hello, World!";
    /// char target[14] = {};
    ///
    /// // Equivalent to strcpy((char*) target, (char*) source)
    /// Util::Address(target).copyString(source);
    /// ```
    void copyString(const char *sourceString) const;

    /// Copy the memory from the given address to the memory this object points to.
    /// Copying is done until a null terminator (0) is reached on the source
    /// or the given maximum number of bytes is reached.
    /// If a null-terminator is reached, the remaining bytes up to `maxBytes` are filled with 0.
    /// If `maxBytes` is reached, the copied string is not null-terminated.
    ///
    /// ### Example
    /// ```c++
    /// auto source = "Hello";
    /// char target[10] = {};
    ///
    /// // Equivalent to strncpy((char*) target, (char*) source, 10)
    /// // target = { 'H', 'e', 'l', 'l', 'o', 0, 0, 0, 0, 0 }
    /// Util::Address(target).copyString(Util::Address(source));
    /// ```
    void copyString(const Address &sourceAddress, size_t maxBytes) const;

    /// Search for the given character in the memory this object points to.
    /// If the character is found, the address of the first occurrence is returned.
    /// Otherwise, an Address object pointing to 0 is returned.
    [[nodiscard]] Address searchCharacter(char character) const;

private:

    size_t address = 0;
};

}

#endif
