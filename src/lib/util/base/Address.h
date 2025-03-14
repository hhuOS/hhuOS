/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * uint32_this program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * uint32_this program is distributed in the hope that it will be useful, but WIuint32_tHOUuint32_t ANY WARRANuint32_tY; without even the implied
 * warranty of MERCHANuint32_tABILIuint32_tY or FIuint32_tNESS FOR A PARuint32_tICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef __Address_include__
#define __Address_include__

#include <stdint.h>

namespace Util {

class Address {

public:

    Address() = default;

    Address(const Address &other) = default;

    Address& operator=(const Address &other) = default;
	
	explicit Address(void *pointer);

    explicit Address(const void *pointer);

    explicit Address(uint32_t address);
    
    virtual ~Address() = default;

    bool operator==(const Address &other) const;

    bool operator!=(const Address &other) const;

    bool operator==(uint32_t otherAddress) const;

    bool operator!=(uint32_t otherAddress) const;

    explicit operator uint32_t() const;

    [[nodiscard]] uint32_t get() const;

    [[nodiscard]] Address set(uint32_t newAddress) const;

    [[nodiscard]] Address add(uint32_t value) const;

    [[nodiscard]] Address subtract(uint32_t value) const;

    [[nodiscard]] Address alignUp(uint32_t alignment) const;

    [[nodiscard]] uint32_t stringLength() const;

    [[nodiscard]] int32_t compareRange(const Address &otherAddress, uint32_t length) const;

    [[nodiscard]] int32_t compareString(const Address &otherAddress) const;

    [[nodiscard]] int32_t compareString(const char *otherString) const;

    [[nodiscard]] uint8_t getByte(uint32_t offset = 0) const;

    [[nodiscard]] uint16_t getShort(uint32_t offset = 0) const;

    [[nodiscard]] uint32_t getInt(uint32_t offset = 0) const;

    [[nodiscard]] uint64_t getLong(uint32_t offset = 0) const;

    void setByte(uint8_t value, uint32_t offset = 0) const;

    void setShort(uint16_t value, uint32_t offset = 0) const;

    void setInt(uint32_t value, uint32_t offset = 0) const;

    void setLong(uint64_t value, uint32_t offset = 0) const;

    virtual void setRange(uint8_t value, uint32_t length) const;

    virtual void copyRange(const Address &sourceAddress, uint32_t length) const;

    void copyString(const Address &sourceAddress) const;

    void copyString(const Address &sourceAddress, uint32_t maxBytes) const;

    [[nodiscard]] Address searchCharacter(uint8_t character) const;

protected:

    uint32_t address = 0;
};

}

#endif
