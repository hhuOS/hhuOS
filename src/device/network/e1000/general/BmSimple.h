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

#ifndef HHUOS_BMSIMPLE_H
#define HHUOS_BMSIMPLE_H

#include "BitManipulation.h"

/**
 * This class implements the BitManipulation interface.
 * This class is meant to be wrapped with a class of type
 * Register, to operate on.
 */
template <typename T>
class BmSimple final : public BitManipulation<T>{
public:
    explicit BmSimple();
    ~BmSimple() override = default;

private:
    /**
     * Values to be set on (to 1) are stored here
     * on the same position with 1.
     * e.g.: Set bits 1 and 2 on: 0...0110.
     */
    T setOn;

    /**
     * Values to be set off (to 0) are stored here
     * on the same position with 1.
     * e.g.: Set bits 1 and 2 off: 0...0110.
     */
    T setOff;

    /**
     * Inherited methods from BitManipulation.
     * This methods are meant to be overridden and
     * implemented by this class.
     */
    void set(T values, T mask) final;
    void decide(T values, bool enable) final;
    T applyTo(T target) final;
};

template<typename T>
BmSimple<T>::BmSimple() : setOn(0), setOff(0) {}

template <typename T>
void BmSimple<T>::set(T values, T mask) {
    decide(mask, false);
    decide(mask & values, true);
}

template <typename T>
void BmSimple<T>::decide(T values, bool enable) {
    enable ? setOn |= values : setOff |= values;
}

template <typename T>
T BmSimple<T>::applyTo(T target) {
    T result = (target & ~setOff) | setOn;
    setOn = 0;
    setOff = 0;
    return result;
}

#endif //HHUOS_BMSIMPLE_H
