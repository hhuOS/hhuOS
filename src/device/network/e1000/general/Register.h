/*
 * Copyright (C) 2018/19 Thiemo Urselmann
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
 *
 * Note:
 * All references marked with [...] refer to the following developers manual.
 * Intel Corporation. PCI/PCI-X Family of Gigabit Ethernet Controllers Software Developer’s Manual.
 * 317453006EN.PDF Revision 4.0. 2009.
 */

#ifndef HHUOS_REGISTER_H
#define HHUOS_REGISTER_H

#include <cstdint>

/**
 * This is an interface for MMIO-register.
 *
 * Classes who operate on MMIO-registers in that kind of way,
 * that they have to manipulate single bits of it,
 * should extend/implement this interface.
 */
class Register {
public:
    virtual ~Register() = default;

    Register(Register const &) = delete;
    Register &operator=(Register const &) = delete;

    /**
     * Reads directly from a register.
     * @return Current value hold by the register.
     */
    virtual uint32_t readDirect() = 0;

    /**
     * Specifies which bits in values should be applied to
     * the register.
     * @param values Contains bits to apply to the register.
     * @param mask Bits set to 1 specify the position in the
     * values parameter which should be applied to the register.
     */
    virtual void set(uint32_t values, uint32_t mask) = 0;

    /**
     * Specifies which bits should be enabled or disabled
     * in the register.
     * @param values Bits set to 1 specify the relevant bits in the register.
     * @param enable If true, specifies that bits in the register should
     * be set to 1 according to the positions of the Bits in values which
     * are also set to 1. Otherwise it specifies that the register positions
     * masked by values, should be set to 0.
     */
    virtual void decide(uint32_t values, bool enable) = 0;

    /**
     * Calling this method applies the specified (via
     * the set- and decide-methods) bits to the current value in the register.
     */
    virtual void confirm() = 0;

protected:
    Register() = default;

private:;
    /**
     * Writes directly to the register.
     * @param values Bits to write to the register.
     */
    virtual void writeDirect(uint32_t values) = 0;
};


#endif //HHUOS_REGISTER_H
