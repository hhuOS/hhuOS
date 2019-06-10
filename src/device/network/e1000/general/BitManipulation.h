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

#ifndef HHUOS_BITMANIPULATION_H
#define HHUOS_BITMANIPULATION_H

/**
 * This is an interface for manipulating several bits of a bit-sequence (bitvector).
 *
 * One chooses and collects bits to set on/off and applies them to a bitvector.
 * @tparam T Defines the size of an bitvector. Datatypes uintx_t
 * with x = 8,16,32 or 64 should be used.
 */
template <typename T>
class BitManipulation {
public:
    virtual ~BitManipulation() = default;

    BitManipulation(BitManipulation const &) = delete;
    BitManipulation &operator=(BitManipulation const &) = delete;

    /**
     * Enables all masked bits from values.
     * @param values Combination of bits to enable.
     * @param mask Bits with 1 specify those bits in values (those who are on the same position)
     * which will be applied by calling the applyTo-method.
     */
    virtual void set(T values, T mask) = 0;

    /**
     * Decides which Bits to set to 0 or 1 when the applyTo-method is called.
     * @param values Determines, due setting bits to 1, which positions of the later target
     * should be affected.
     * @param enable if true, sets those bits of the target to 1 which are masked by values.
     * Otherwise they will be set to 0.
     */
    virtual void decide(T values, bool enable) = 0;

    /**
     * Applies those bits on target, which were configured by the set-and decide-Method.
     * Internal buffered/stored values for computing the result should be set to 0 (reset).
     * @param target The value on which the configured bits will be applied.
     * @return  The result after applying the configured bits on the target.
     */
    virtual T applyTo(T target) = 0;

protected:
    BitManipulation() = default;
};



#endif //HHUOS_BITMANIPULATION_H
