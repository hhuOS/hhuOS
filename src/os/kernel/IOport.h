/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#ifndef __IOport_include__
#define __IOport_include__

#include <cstdint>


class IOport {

    // 16 bit address in IO address space where this port is placed
    uint16_t address;

public:
    /**
     * Constructor, takes address of this port
     */
    IOport (uint16_t a) noexcept : address (a) { };

    /**
     * Writes a byte to IO-port
     *
     * @param val 8-bit value to write.
     */
    void outb (uint8_t val) const {
        asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(address) );
    }
    
    /**
	 * Writes a byte to IO-port with an offset to port address
	 *
	 * @param addr_offset Offset to port address
	 * @param val 8-bit value to write.
	 */
	void outb (uint16_t addr_offset, uint8_t val) const {
		uint16_t addr = address + addr_offset;
		asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(addr) );
    }
    	
	/**
	 * Writes a word to IO-port.
	 *
	 * @param val 16-bit value to write.
	 */
    void outw (uint16_t val) const {
        asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(address) );
    }

    /**
	 * Writes a doubleword (32 bit) to IO-port.
	 *
	 * @param val 32-bit value to write.
	 */
    void outdw (uint32_t val) const {
        asm volatile ( "outl %0, %1" : : "a"(val), "Nd"(address) );
    }

    /**
	 * Reads a byte from IO-port.
	 *
	 * @return 8-bit value from IO-port
	 */
    uint8_t inb () const {
        uint8_t ret;

        asm volatile ( "inb %1, %0"
                        : "=a"(ret)
                        : "Nd"(address) );
        return ret;
    }
    
    /**
	 * Reads a byte from IO-port with offset
	 *
	 * @param addr_offset Offset to port address
	 * @return 8-bit value from IO-port
	 */
	uint8_t inb (uint16_t addr_offset) const {
		uint8_t ret;
		uint16_t addr = address + addr_offset;
		asm volatile ( "inb %1, %0"
						: "=a"(ret)
						: "Nd"(addr) );
		return ret;
	}
	
	/**
	 * Reads a word from IO-port.
	 *
	 * @return 16-bit value from IO-port
	 */
    uint16_t inw () const {
        uint16_t ret;

        asm volatile ( "inw %1, %0"
                        : "=a"(ret)
                        : "Nd"(address) );
        return ret;
    }
    
    /**
	 * Reads a a doubleword (32 bit) from IO-port.
	 *
	 * @return 32-bit value from IO-port
	 */
    uint32_t indw () const {
        uint32_t ret;
        
        asm volatile ( "inl %1, %0"
                      : "=a"(ret)
                      : "Nd"(address) );
        return ret;
    }

    /**
     * Returns address of this IO-port.
     *
     * @return Address of the current port
     */
    uint16_t getAddress() {
        return address;
    }
};

#endif
