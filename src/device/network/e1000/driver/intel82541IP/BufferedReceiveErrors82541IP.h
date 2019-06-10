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

#ifndef HHUOS_BUFFEREDRECEIVEERRORS82541IP_H
#define HHUOS_BUFFEREDRECEIVEERRORS82541IP_H


#include "device/cpu/Cpu.h"
#include "device/network/e1000/receive/descriptor/field/ReBufferedDefault.h"
/**
 * This class implements the abstract class ReBuffered.
 *
 * It's implementation depends on the Intel 82541IP card.
 */
class BufferedReceiveErrors82541IP final : public ReBufferedDefault {
public:
    /**
      * Constructor. Same as in extended class.
      */
    explicit BufferedReceiveErrors82541IP(uint8_t *address);
    ~BufferedReceiveErrors82541IP() override = default;

private:
    /**
      * Inherited methods from ReBuffered.
      * This methods are meant to be overridden and
      * implemented by this class.
      */
    bool symbolError() final;
    bool sequenceError() final;
    bool carrierExtensionError() final;
    bool rxDataError() final;
    bool hasErrors() final;
};


#endif //HHUOS_BUFFEREDRECEIVEERRORS82541IP_H
