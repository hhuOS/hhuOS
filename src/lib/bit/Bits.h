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

#ifndef __Bits_include__
#define __Bits_include__

#include <cstdint>

struct BitField {
    uint8_t     off;
    uint8_t     len;

    BitField(uint8_t offset, uint8_t length) : off(offset), len(length) {}
};

static inline uint32_t bit(uint8_t position) {
    return (1U << position);
}

static inline uint32_t bitMask(uint8_t length) {
    return bit(length) - 1U;
}

static inline uint32_t bitMask(BitField bitField) {
    return (bit(bitField.len) - 1U) << bitField.off;
}

static inline volatile uint32_t& setBit(volatile uint32_t &reg, uint8_t position) {
    return reg |= bit(position);
}

static inline uint32_t& setBit(uint32_t &reg, uint8_t position) {
    return reg |= bit(position);
}

static inline volatile uint32_t& clearBit(volatile uint32_t &reg, uint8_t position) {
    return reg &= ~bit(position);
}

static inline uint32_t& clearBit(uint32_t &reg, uint8_t position) {
    return reg &= ~bit(position);
}

static inline volatile uint32_t& clearValue(volatile uint32_t &reg, BitField bitField) {
    return reg &= ~bitMask(bitField);
}

static inline uint32_t& clearValue(uint32_t &reg, BitField bitField) {
    return reg &= ~bitMask(bitField);
}

static inline volatile uint32_t& setValue(volatile uint32_t &reg, BitField bitField, uint32_t value) {
    return reg |= clearValue(reg, bitField) | (value << bitField.off);
}

static inline uint32_t& setValue(uint32_t &reg, BitField bitField, uint32_t value) {
    return reg |= clearValue(reg, bitField) | (value << bitField.off);
}

static inline volatile uint32_t getValue(volatile uint32_t &reg, BitField bitField) {
    return (reg & bitMask(bitField)) >> bitField.off;
}

static inline uint32_t getValue(uint32_t &reg, BitField bitField) {
    return (reg & bitMask(bitField)) >> bitField.off;
}

#endif
