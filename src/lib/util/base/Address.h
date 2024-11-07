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

#ifndef __Address_include__
#define __Address_include__

#include <stdint.h>

namespace Util {

template<typename T>
class Address {

public:

    Address() = default;

    Address(const Address &other) = default;

    Address& operator=(const Address &other) = default;
	
	explicit Address(void *pointer);

    explicit Address(const void *pointer);

    explicit Address(T address);

    
    virtual ~Address() = default;

    bool operator==(const Address &other) const;

    bool operator!=(const Address &other) const;

    bool operator==(T otherAddress) const;

    bool operator!=(T otherAddress) const;

    explicit operator T() const;

    [[nodiscard]] T get() const;

    [[nodiscard]] Address<T> set(T newAddress) const;

    [[nodiscard]] Address<T> add(T value) const;

    [[nodiscard]] Address<T> subtract(T value) const;

    [[nodiscard]] Address<T> alignUp(T alignment) const;

    [[nodiscard]] T stringLength() const;

    [[nodiscard]] int32_t compareRange(const Address<T> &otherAddress, T length) const;

    [[nodiscard]] int32_t compareString(const Address<T> &otherAddress) const;

    [[nodiscard]] int32_t compareString(const char *otherString) const;

    [[nodiscard]] uint8_t getByte(T offset = 0) const;

    [[nodiscard]] uint16_t getShort(T offset = 0) const;

    [[nodiscard]] uint32_t getInt(T offset = 0) const;

    [[nodiscard]] uint64_t getLong(T offset = 0) const;

    void setByte(uint8_t value, T offset = 0) const;

    void setShort(uint16_t value, T offset = 0) const;

    void setInt(uint32_t value, T offset = 0) const;

    void setLong(uint64_t value, T offset = 0) const;

    virtual void setRange(uint8_t value, T length) const;

    virtual void copyRange(const Address<T> &sourceAddress, T length) const;

    void copyString(const Address<T> &sourceAddress) const;

    void copyString(const Address<T> &sourceAddress, T maxBytes) const;

    [[nodiscard]] Address<T> searchCharacter(uint8_t character) const;

protected:

    T address{};
};

template
class Address<uint16_t>;

template
class Address<uint32_t>;

}

#endif
