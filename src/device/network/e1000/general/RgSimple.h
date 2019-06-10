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

#ifndef HHUOS_RGSIMPLE_H
#define HHUOS_RGSIMPLE_H

#include "Register.h"
#include "BitManipulation.h"

/**
 * This class implements the Register interface.
 *
 * Collect options due using this methods and apply them
 * with the confirm method to the register.
 */
class RgSimple final : public Register {
public:
    /**
     * Constructor.
     * @param location The address of the according register in the MMIO-space.
     * @param manipulation Encapsulates access to a 32-bit register.
     */
    explicit RgSimple(uint32_t *location, BitManipulation<uint32_t > *manipulation);
    ~RgSimple() override = default;

private:
    /**
     * The address of the according register in the MMIO-space.
     * Reads from this location should always be directly from the register.
     */
    volatile uint32_t *location;

    /**
     * Encapsulates access to a register.
     */
    BitManipulation<uint32_t> *manipulation;

    /**
     * Inherited methods from Register.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void writeDirect(uint32_t values) final;
    uint32_t readDirect() final;
    void set(uint32_t values, uint32_t mask) final;
    void decide(uint32_t values, bool enable) final;
    void confirm() final;
};


#endif //HHUOS_RGSIMPLE_H
