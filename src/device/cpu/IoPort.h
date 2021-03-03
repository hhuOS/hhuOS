/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef __IoPort_include__
#define __IoPort_include__

#include <cstdint>
#include <util/memory/Address.h>

namespace Device {

/**
 * Represents an IO-port int the 16-bit address space.
 */
class IoPort {

public:
    /**
     * Constructor.
     *
     * @param address The 16-bit address in the IO address space, at which this port is located
     */
    explicit IoPort(uint16_t address) noexcept;

    /**
     * Copy constructor.
     */
    IoPort(const IoPort &other) = delete;

    /**
     * Assignment operator.
     */
    IoPort &operator=(const IoPort &other) = delete;

    /**
     * Destructor.
     */
    ~IoPort() = default;

    /**
     * Returns the address of this IO-port.
     *
     * @return The address of this port
     */
    [[nodiscard]] Util::Memory::Address<uint16_t> getAddress() const;

    /**
     * Write a byte to this IO-port.
     *
     * @param value 8-bit value to write
     */
    void writeByte(uint8_t value) const;

    /**
	 * Write a byte to this IO-port this with an offset to the address.
	 *
	 * @param offset Offset to port address
	 * @param val 8-bit value to write
	 */
    void writeByte(uint16_t offset, uint8_t val) const;

    /**
     * Write a word to this IO-port.
     *
     * @param value 16-bit value to write
     */
    void writeWord(uint16_t value) const;

    /**
	 * Write a double word to this IO-port.
	 *
	 * @param value 32-bit value to write
	 */
    void writeDoubleWord(uint32_t value) const;

    /**
	 * Read a byte from this IO-port.
	 *
	 * @return 8-bit value read from IO-port
	 */
    [[nodiscard]] uint8_t readByte() const;

    /**
	 * Read a byte from this IO-port with an offset to the address.
	 *
	 * @param offset Offset to port address
	 * @return 8-bit value read from IO-port
	 */
    [[nodiscard]] uint8_t readByte(uint16_t offset) const;

    /**
     * Read a word from this IO-port.
     *
     * @return 16-bit value read from IO-port
     */
    [[nodiscard]] uint16_t readWord() const;

    /**
	 * Read a word from this IO-port with an offset to the address.
	 *
	 * @param offset Offset to port address
	 * @return 8-bit value read from IO-port
	 */
    [[nodiscard]] uint16_t readWord(uint16_t offset) const;

    /**
	 * Read a double word from this IO-port.
	 *
	 * @return 32-bit value read from IO-port
	 */
    [[nodiscard]] uint32_t readDoubleWord() const;

    /**
	 * Read a double word from this IO-port with an offset to the address.
	 *
	 * @param offset Offset to port address
	 * @return 8-bit value from IO-port
	 */
    [[nodiscard]] uint32_t readDoubleWord(uint16_t offset) const;

private:

    const Util::Memory::Address<uint16_t> address;
};

}

#endif
